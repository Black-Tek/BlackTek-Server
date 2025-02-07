// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#include "otpch.h"

#include "outputmessage.h"
#include "server.h"
#include "scheduler.h"
#include "configmanager.h"
#include "ban.h"

extern ConfigManager g_config;
Ban g_bans;

ServiceManager::~ServiceManager()
{
	stop();
}

void ServiceManager::die()
{
	io_context.stop();
}

void ServiceManager::run()
{
	assert(!running);
	running = true;
	io_context.run();
}

void ServiceManager::stop()
{
	if (!running) {
		return;
	}

	running = false;

	for (auto& servicePortIt : acceptors) {
		try {
			boost::asio::post(io_context, [servicePort = servicePortIt.second]() { servicePort->onStopServer(); });
		} catch (boost::system::system_error& e) {
			std::cout << "[ServiceManager::stop] Network Error: " << e.what() << std::endl;
		}
	}

	acceptors.clear();

	death_timer.expires_after(std::chrono::seconds(3));
	death_timer.async_wait([this](const boost::system::error_code&) { die(); });
}

ServicePort::~ServicePort()
{
	close();
}

bool ServicePort::is_single_socket() const
{
	return !services.empty() && services.front()->is_single_socket();
}

std::string ServicePort::get_protocol_names() const
{
	if (services.empty()) {
		return std::string();
	}

	std::string str = services.front()->get_protocol_name();
	for (size_t i = 1; i < services.size(); ++i) {
		str.push_back(',');
		str.push_back(' ');
		str.append(services[i]->get_protocol_name());
	}
	return str;
}

void ServicePort::accept()
{
	if (!acceptor) {
		return;
	}

	auto connection = ConnectionManager::getInstance().createConnection(io_context, shared_from_this());
	acceptor->async_accept(connection->getSocket(), [=, thisPtr = shared_from_this()](const boost::system::error_code& error) { thisPtr->onAccept(connection, error); });
}

void ServicePort::onAccept(const Connection_ptr& connection, const boost::system::error_code& error)
{
	if (!error) {
		if (services.empty()) {
			return;
		}

		auto remote_ip = connection->getIP();
		if (remote_ip != 0 && g_bans.acceptConnection(remote_ip)) {
			Service_ptr service = services.front();
			if (service->is_single_socket()) {
				connection->accept(service->make_protocol(connection));
			} else {
				connection->accept();
			}
		} else {
			connection->close(Connection::FORCE_CLOSE);
		}

		accept();
	} else if (error != boost::asio::error::operation_aborted) {
		if (!pendingStart) {
			close();
			pendingStart = true;
			g_scheduler.addEvent(createSchedulerTask(15000, [=, thisPtr = std::weak_ptr<ServicePort>(shared_from_this())]() { ServicePort::openAcceptor(thisPtr, serverPort); }));
		}
	}
}

Protocol_ptr ServicePort::make_protocol(bool checksummed, NetworkMessage& msg, const Connection_ptr& connection) const
{
	uint8_t protocolID = msg.getByte();
	for (auto& service : services) {
		if (protocolID != service->get_protocol_identifier()) {
			continue;
		}

		if ((checksummed && service->is_checksummed()) || !service->is_checksummed()) {
			return service->make_protocol(connection);
		}
	}
	return nullptr;
}

void ServicePort::onStopServer()
{
	close();
}

void ServicePort::openAcceptor(const std::weak_ptr<ServicePort>& weak_service, uint16_t port)
{
	if (auto service = weak_service.lock()) {
		service->open(port);
	}
}

void ServicePort::open(uint16_t port)
{
	close();

	serverPort = port;
	pendingStart = false;

	try {
		if (g_config.getBoolean(ConfigManager::BIND_ONLY_GLOBAL_ADDRESS)) {
			acceptor.reset(new boost::asio::ip::tcp::acceptor(io_context, boost::asio::ip::tcp::endpoint(
			            boost::asio::ip::address(boost::asio::ip::make_address(g_config.getString(ConfigManager::IP))), serverPort)));
		} else {
			acceptor.reset(new boost::asio::ip::tcp::acceptor(io_context, boost::asio::ip::tcp::endpoint(
			            boost::asio::ip::address(boost::asio::ip::address_v4(INADDR_ANY)), serverPort)));
		}

		acceptor->set_option(boost::asio::ip::tcp::no_delay(true));

		accept();
	} catch (boost::system::system_error& e) {
		std::cout << "[ServicePort::open] Error: " << e.what() << std::endl;

		pendingStart = true;
		g_scheduler.addEvent(createSchedulerTask(15000, [=, thisPtr = std::weak_ptr<ServicePort>(shared_from_this())]() { ServicePort::openAcceptor(thisPtr, serverPort); }));
	}
}

void ServicePort::close() const
{
	if (acceptor && acceptor->is_open()) {
		boost::system::error_code error;
		acceptor->close(error);
	}
}

bool ServicePort::add_service(const Service_ptr& new_svc)
{
	if (std::any_of(services.begin(), services.end(), [](const Service_ptr& svc) {return svc->is_single_socket();})) {
		return false;
	}

	services.push_back(new_svc);
	return true;
}
