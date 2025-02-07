// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_PROTOCOLOLD_H
#define FS_PROTOCOLOLD_H

#include "protocol.h"

class NetworkMessage;

class ProtocolOld final : public Protocol
{
	public:
		// static protocol information
		enum {server_sends_first = false};
		enum {protocol_identifier = 0x01};
		enum {use_checksum = false};
		static const char* protocol_name() {
			return "old login protocol";
		}
		// todo: use reference on connection
		explicit ProtocolOld(Connection_ptr connection) : Protocol(connection) {}

		void onRecvFirstMessage(NetworkMessage& msg) override;

	private:
		void disconnectClient(const std::string& message);
};

#endif
