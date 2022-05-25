#pragma once
#include <string>
#include <thread>
#include "NetDefs.h"

#include <iostream>

namespace ecse::Networking
{
	/// <summary>
	/// UDP listener for all incoming datagrams on the specified port.
	/// </summary>
	class Server
	{
	private:
		asio::io_context _Context;
		asio::ip::udp::socket _Socket;
		asio::ip::udp::endpoint _SenderEndpoint;
		std::thread* _Thread;
		std::vector<char> _Buffer;
		
		void HandleReceive(const asio::error_code& error, size_t bytes_transferred);
		void Listen();
		
	public:
		Server(asio::ip::port_type port)
			: _Socket(_Context, asio::ip::udp::endpoint(asio::ip::udp::v4(), port)), _Thread(nullptr), _Buffer(1024) {}
		
		static std::shared_ptr<Server> Start(asio::ip::port_type port);

		void Stop();
	};
}