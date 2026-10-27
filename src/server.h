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

#ifndef FS_SERVER_H
#define FS_SERVER_H

#include "connection.h"
#include "signals.h"

#include <memory>
#include <gtl/phmap.hpp>

class Protocol;

class ServiceBase
{
	public:
		virtual bool is_single_socket() const = 0;
		virtual bool is_checksummed() const = 0;
		virtual uint8_t get_protocol_identifier() const = 0;
		virtual const char* get_protocol_name() const = 0;

		virtual Protocol_ptr make_protocol(const Connection_ptr& c) const = 0;
};

template <typename ProtocolType>
class Service final : public ServiceBase
{
	public:
		bool is_single_socket() const override {
			return ProtocolType::server_sends_first;
		}
		bool is_checksummed() const override {
			return ProtocolType::use_checksum;
		}
		uint8_t get_protocol_identifier() const override {
			return ProtocolType::protocol_identifier;
		}
		const char* get_protocol_name() const override {
			return ProtocolType::protocol_name();
		}

		Protocol_ptr make_protocol(const Connection_ptr& c) const override {
			return std::make_shared<ProtocolType>(c);
		}
};

class ServicePort : public std::enable_shared_from_this<ServicePort>
{
	public:
		explicit ServicePort(boost::asio::io_context& io_context) : io_context(io_context) {}
		~ServicePort();

		// non-copyable
		ServicePort(const ServicePort&) = delete;
		ServicePort& operator=(const ServicePort&) = delete;

		static void openAcceptor(const std::weak_ptr<ServicePort>& weak_service, uint16_t port);
		void open(uint16_t port);
		void close() const;
		bool is_single_socket() const;
		std::string get_protocol_names() const;

		bool add_service(const Service_ptr& new_svc);
		Protocol_ptr make_protocol(bool checksummed, NetworkMessage& msg, const Connection_ptr& connection) const;

		void onStopServer();
		void onAccept(const Connection_ptr& connection, const boost::system::error_code& error);

	private:
		void accept();

		boost::asio::io_context& io_context;
		std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor;
		std::vector<Service_ptr> services;

		uint16_t serverPort = 0;
		bool pendingStart = false;
};

class ServiceManager
{
	public:
		ServiceManager() = default;
		~ServiceManager();

		// non-copyable
		ServiceManager(const ServiceManager&) = delete;
		ServiceManager& operator=(const ServiceManager&) = delete;

		void run();
		void stop();

		template <typename ProtocolType>
		bool add(uint16_t port);

		bool is_running() const {
			return acceptors.empty() == false;
		}

	private:
		void die();

		gtl::node_hash_map<uint16_t, ServicePort_ptr> acceptors;

		boost::asio::io_context io_context;
		Signals signals{io_context};
		boost::asio::steady_timer death_timer { io_context };
		bool running = false;
};

template <typename ProtocolType>
bool ServiceManager::add(uint16_t port)
{
	if (port == 0) {
		std::cout << "ERROR: No port provided for service " << ProtocolType::protocol_name() << ". Service disabled." << std::endl;
		return false;
	}

	ServicePort_ptr service_port;

	auto foundServicePort = acceptors.find(port);

	if (foundServicePort == acceptors.end()) {
		service_port = std::make_shared<ServicePort>(io_context);
		service_port->open(port);
		acceptors[port] = service_port;
	} else {
		service_port = foundServicePort->second;

		if (service_port->is_single_socket() || ProtocolType::server_sends_first) {
			std::cout << "ERROR: " << ProtocolType::protocol_name() <<
			          " and " << service_port->get_protocol_names() <<
			          " cannot use the same port " << port << '.' << std::endl;
			return false;
		}
	}

	return service_port->add_service(std::make_shared<Service<ProtocolType>>());
}

#endif
