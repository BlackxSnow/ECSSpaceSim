#pragma once

#include "../NetDefs.h"
#include "../Packet.h"
#include <queue>

namespace ecse::Networking::tcp
{
	class Connection
	{
	private:
		asio::io_context _Context;
		std::thread* _Thread;
		asio::ip::tcp::endpoint _Endpoint;
		asio::ip::tcp::socket _Socket;

		std::queue<std::shared_ptr<AggregatePacket>> _SendQueue;
		std::vector<byte> _ReceiveBuffer;

		void Listen();

		void HandleReceive(const asio::error_code& error, size_t bytesTransferred);

	public:
		const asio::ip::tcp::endpoint& Endpoint() { return _Endpoint; }

		void Send(Packet& packet);
		void Flush();

		Connection(const asio::ip::tcp::endpoint& local, const asio::ip::address& address, const asio::ip::port_type port) : _Socket(_Context), _ReceiveBuffer(1024)
		{
			asio::ip::tcp::endpoint endpoint = *asio::ip::tcp::resolver(_Context).resolve(asio::ip::tcp::endpoint(address, port)).begin();
			
			_Socket.open(endpoint.protocol());
			_Socket.bind(local);
			_Socket.connect(endpoint);
			
			_Endpoint = _Socket.remote_endpoint();

			Listen();
			_Thread = new std::thread([&]() { _Context.run(); });
		}
		Connection(asio::ip::tcp::socket& socket) : _Socket(_Context), _ReceiveBuffer(1024)
		{
			auto native = socket.native_handle();
			auto handle = socket.release();
			_Socket.assign(asio::ip::tcp::v4(), native);

			_Endpoint = _Socket.remote_endpoint();

			Listen();

			_Thread = new std::thread([&]() { _Context.run(); });
		};
		
		~Connection()
		{
			_Socket.cancel();
			_Context.stop();
			_Socket.close();
			_Thread->join();
			delete _Thread;
		}
	};

	class Listener
	{
	private:
		asio::io_context _Context;
		std::thread* _Thread;
		asio::ip::tcp::acceptor _Acceptor;
		std::function<void(std::shared_ptr<Connection>)> _OnAccept;
		
		void HandleAccept(const asio::error_code& ec, asio::ip::tcp::socket socket)
		{
			if (ec)
			{
				if (ec.value() == asio::error::operation_aborted)
				{
					LogInfo((std::ostringstream() << "Aborting listen on port " << _Acceptor.local_endpoint().port()).str());
					return;
				}
				LogError(ec.message(), false);
				return;
			}
			
			_OnAccept(std::make_shared<Connection>(socket));
		}

	public:
		Listener(asio::ip::port_type port, std::function<void(std::shared_ptr<Connection>)> onAccept) : _Acceptor(_Context), _OnAccept(onAccept)
		{
			asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), port);
			_Acceptor.open(endpoint.protocol());
			_Acceptor.bind(endpoint);
			_Acceptor.listen();
			_Acceptor.async_accept(std::bind(&Listener::HandleAccept, this, std::placeholders::_1, std::placeholders::_2));
			_Thread = new std::thread([&]() { _Context.run(); });
		}
	};
}