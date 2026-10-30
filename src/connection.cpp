/**
 * BlackTek Server Engine
 * https://github.com/Black-Tek/BlackTek-Server
 *
 * ============================================================================
 * COPYRIGHT & ATTRIBUTION NOTICE
 * ============================================================================
 *
 * Upstream Copyright:
 *   Copyright (C) The Forgotten Server Authors
 *   Governed by the GNU General Public License v2.0 (GPL-2.0)
 *
 * Modifications & BlackTek Core Additions:
 *   Copyright (C) 2024-2026 BlackTek (Author: Codinablack)
 *
 * ============================================================================
 * DUAL LICENSING & TRANSITION NOTICE
 * ============================================================================
 *
 * This file contains original code from The Forgotten Server alongside substantial
 * modifications and new architecture by BlackTek.
 *
 * To facilitate the project's long-term goal of transitioning the codebase to a
 * fully permissive MIT License:
 *
 * 1. All original TFS source lines remain governed by GPL-2.0.
 * 2. All contributions, modifications, and new additions authored by BlackTek
 *    are dual-licensed under both the MIT License and GPL-2.0.
 * 3. Downstream users distributing this combined codebase must comply with
 *    the GPL-2.0 requirements as long as original upstream TFS code remains.
 *
 * Once an individual module or file has been completely refactored or rewritten
 * such that no original TFS code remains, the file will be relicensed solely
 * under the MIT License.
 *
 * See licensing/original codebase/GNU-GPL (GPL-2.0) and licensing/License (MIT)
 * in the project root for the full texts.
 */

#include "otpch.h"

#include "configmanager.h"
#include "connection.h"
#include "outputmessage.h"
#include "protocol.h"
#include "scheduler.h"
#include "server.h"

extern ConfigManager g_config;

Connection_ptr ConnectionManager::createConnection(boost::asio::io_context& io_context, ConstServicePort_ptr servicePort)
{
	auto connection = std::make_shared<Connection>(io_context, servicePort);
	connections.insert(connection);
	return connection;
}

void ConnectionManager::releaseConnection(const Connection_ptr& connection)
{
	connections.erase(connection);
}

void ConnectionManager::closeAll()
{
	connections.for_each([](const Connection_ptr& connection)
	{
		try
		{
			boost::system::error_code error;
			connection->socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
			connection->socket.close(error);
		}
		catch (boost::system::system_error&) {}
	});
	connections.clear();
}

// Connection

void Connection::close(bool force)
{
	// dispatch() executes inline when already on-strand, otherwise posts.
	// This eliminates recursive locking while remaining safe from any thread.
	boost::asio::dispatch(strand, [this, self = shared_from_this(), force]()
	{
		if (closed)
		{
			if (force)
				closeSocket();
			return;
		}
		closed = true;

		ConnectionManager::getInstance().releaseConnection(self);

		if (protocol)
		{
			g_dispatcher.addTask(createTask([protocol = protocol]() { protocol->release(); }));
		}

		if (messageQueue.empty() or force)
		{
			closeSocket();
		}
		// else closeSocket() called from onWriteOperation once queue drains
	});
}

void Connection::closeSocket()
{
	if (socket.is_open())
	{
		try
		{
			readTimer.cancel();
			writeTimer.cancel();
			boost::system::error_code error;
			socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
			socket.close(error);
		}
		catch (boost::system::system_error& e)
		{
			std::cout << "[Network error - Connection::closeSocket] " << e.what() << std::endl;
		}
	}
}

Connection::~Connection()
{
	closeSocket();
}

void Connection::accept(Protocol_ptr protocol)
{
	this->protocol = protocol;
	g_dispatcher.addTask(createTask([=]() { protocol->onConnect(); }));

	accept();
}

void Connection::accept()
{
	try
	{
		msg = NetworkMessagePool::getNetworkMessage();

		readTimer.expires_after(std::chrono::seconds(CONNECTION_READ_TIMEOUT));
		readTimer.async_wait(
			boost::asio::bind_executor(strand,
				[thisPtr = std::weak_ptr<Connection>(shared_from_this())](const boost::system::error_code& error)
				{
					Connection::handleTimeout(thisPtr, error);
				}));

		boost::asio::async_read(socket,
			boost::asio::buffer(msg->getBuffer(), NetworkMessage::HEADER_LENGTH),
			boost::asio::bind_executor(strand,
				[thisPtr = shared_from_this()](const boost::system::error_code& error, auto /*bytes_transferred*/)
				{
					thisPtr->parseHeader(error);
				}));
	}
	catch (boost::system::system_error& e)
	{
		std::cout << "[Network error - Connection::accept] " << e.what() << std::endl;
		close(FORCE_CLOSE);
	}
}

void Connection::parseHeader(const boost::system::error_code& error)
{
	readTimer.cancel();

	if (error)
	{
		close(FORCE_CLOSE);
		return;
	}
	else if (closed)
	{
		return;
	}

	uint32_t timePassed = std::max<uint32_t>(1, (time(nullptr) - timeConnected) + 1);

	if ((++packetsSent / timePassed) > static_cast<uint32_t>(g_config.GetNumber(ConfigManager::MAX_PACKETS_PER_SECOND)))
	{
		std::cout << convertIPToString(getIP()) << " disconnected for exceeding packet per second limit." << std::endl;
		close();
		return;
	}

	if (timePassed > 2)
	{
		timeConnected = time(nullptr);
		packetsSent = 0;
	}

	uint16_t size = msg->getLengthHeader();
	if (size == 0 or size >= NETWORKMESSAGE_MAXSIZE - 16)
	{
		close(FORCE_CLOSE);
		return;
	}

	try
	{
		readTimer.expires_after(std::chrono::seconds(CONNECTION_READ_TIMEOUT));
		readTimer.async_wait(
			boost::asio::bind_executor(strand,
				[thisPtr = std::weak_ptr<Connection>(shared_from_this())](const boost::system::error_code& error)
				{
					Connection::handleTimeout(thisPtr, error);
				}));

		msg->setLength(size + NetworkMessage::HEADER_LENGTH);
		boost::asio::async_read(socket,
			boost::asio::buffer(msg->getBodyBuffer(), size),
			boost::asio::bind_executor(strand,
				[thisPtr = shared_from_this()](const boost::system::error_code& error, auto /*bytes_transferred*/)
				{
					thisPtr->parsePacket(error);
				}));
	}
	catch (boost::system::system_error& e)
	{
		std::cout << "[Network error - Connection::parseHeader] " << e.what() << std::endl;
		close(FORCE_CLOSE);
	}
}

void Connection::parsePacket(const boost::system::error_code& error)
{
	readTimer.cancel();

	if (error)
	{
		close(FORCE_CLOSE);
		return;
	}
	else if (closed)
	{
		return;
	}

	uint32_t checksum;
	int32_t len = msg->getLength() - msg->getBufferPosition() - NetworkMessage::CHECKSUM_LENGTH;

	if (len > 0)
	{
		checksum = adlerChecksum(msg->getBuffer() + msg->getBufferPosition() + NetworkMessage::CHECKSUM_LENGTH, len);
	}
	else
	{
		checksum = 0;
	}

	const bool canReadChecksum = len >= 0;
	const uint32_t recvChecksum = canReadChecksum ? msg->get<uint32_t>() : 0;
	const bool hasChecksum = canReadChecksum and recvChecksum == checksum;
	if (canReadChecksum and not hasChecksum)
	{
		msg->skipBytes(-NetworkMessage::CHECKSUM_LENGTH);
	}

	if (not receivedFirst)
	{
		receivedFirst = true;

		if (not protocol)
		{
			protocol = service_port->make_protocol(hasChecksum, *msg, shared_from_this());
			if (not protocol)
			{
				close(FORCE_CLOSE);
				return;
			}
		}
		else
		{
			msg->skipBytes(1); // Skip protocol ID
		}

		protocol->onRecvFirstMessage(*msg);
	}
	else
	{
		protocol->onRecvMessage(*msg); // Send the packet to the current protocol
	}

	try
	{
		readTimer.expires_after(std::chrono::seconds(CONNECTION_READ_TIMEOUT));
		readTimer.async_wait(
			boost::asio::bind_executor(strand,
				[thisPtr = std::weak_ptr<Connection>(shared_from_this())](const boost::system::error_code& error)
				{
					Connection::handleTimeout(thisPtr, error);
				}));

		// Release the consumed buffer and acquire a fresh one for the next packet
		msg = NetworkMessagePool::getNetworkMessage();

		// Wait for the next packet
		boost::asio::async_read(socket,
			boost::asio::buffer(msg->getBuffer(), NetworkMessage::HEADER_LENGTH),
			boost::asio::bind_executor(strand,
				[thisPtr = shared_from_this()](const boost::system::error_code& error, auto /*bytes_transferred*/)
				{
					thisPtr->parseHeader(error);
				}));
	}
	catch (boost::system::system_error& e)
	{
		std::cout << "[Network error - Connection::parsePacket] " << e.what() << std::endl;
		close(FORCE_CLOSE);
	}
}

void Connection::send(const OutputMessage_ptr& msg)
{
	boost::asio::dispatch(strand, [this, self = shared_from_this(), msg]()
	{
		if (closed)
		{
			return;
		}

		bool noPendingWrite = messageQueue.empty();
		messageQueue.emplace_back(msg);
		if (noPendingWrite)
		{
			internalSend(msg);
		}
	});
}

void Connection::internalSend(const OutputMessage_ptr& msg)
{
	protocol->onSendMessage(msg);
	try
	{
		writeTimer.expires_after(std::chrono::seconds(CONNECTION_WRITE_TIMEOUT));
		writeTimer.async_wait(
			boost::asio::bind_executor(strand,
				[thisPtr = std::weak_ptr<Connection>(shared_from_this())](const boost::system::error_code& error)
				{
					Connection::handleTimeout(thisPtr, error);
				}));

		boost::asio::async_write(socket,
			boost::asio::buffer(msg->getOutputBuffer(), msg->getLength()),
			boost::asio::bind_executor(strand,
				[thisPtr = shared_from_this()](const boost::system::error_code& error, auto /*bytes_transferred*/)
				{
					thisPtr->onWriteOperation(error);
				}));
	}
	catch (boost::system::system_error& e)
	{
		std::cout << "[Network error - Connection::internalSend] " << e.what() << std::endl;
		close(FORCE_CLOSE);
	}
}

uint32_t Connection::getIP()
{
	uint32_t cached = cachedIP.load(std::memory_order_relaxed);
	if (cached != 0)
	{
		return cached;
	}

	boost::system::error_code error;
	const boost::asio::ip::tcp::endpoint endpoint = socket.remote_endpoint(error);
	if (error)
	{
		return 0;
	}

	const auto address = endpoint.address();
	uint32_t ip;
	if (address.is_v4())
	{
		ip = htonl(address.to_v4().to_uint());
	}
	else
	{
		const auto addressV6 = address.to_v6();
		if (not addressV6.is_v4_mapped())
			return 0; // Account storage and IP bans currently require an IPv4 address.

		const auto bytes = addressV6.to_bytes();
		ip = htonl((static_cast<uint32_t>(bytes[12]) << 24) |
			(static_cast<uint32_t>(bytes[13]) << 16) |
			(static_cast<uint32_t>(bytes[14]) << 8) | bytes[15]);
	}
	cachedIP.store(ip, std::memory_order_relaxed);
	return ip;
}

void Connection::onWriteOperation(const boost::system::error_code& error)
{
	writeTimer.cancel();
	OutputMessage_ptr msg = messageQueue.front();
	messageQueue.pop_front();

	msg->reset();

	if (error)
	{
		messageQueue.clear();
		close(FORCE_CLOSE);
		return;
	}

	if (not messageQueue.empty())
	{
		internalSend(messageQueue.front());
	}
	else if (closed)
	{
		closeSocket();
	}
}

void Connection::handleTimeout(ConnectionWeak_ptr connectionWeak, const boost::system::error_code& error)
{
	if (error == boost::asio::error::operation_aborted)
	{
		return;
	}

	if (auto connection = connectionWeak.lock())
	{
		connection->close(FORCE_CLOSE);
	}
}
