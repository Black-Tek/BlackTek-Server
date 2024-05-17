// Copyright 2024 Black Tek Server Authors. All rights reserved.
// Use of this source code is governed by the GPL-2.0 License that can be found in the LICENSE file.

#ifndef FS_SIGNALHANDLINGTHREAD_H
#define FS_SIGNALHANDLINGTHREAD_H

#include <boost/asio.hpp>

class Signals
{
	boost::asio::signal_set set;
	public:
		explicit Signals(boost::asio::io_context& context);

	private:
		void asyncWait();
};

#endif
