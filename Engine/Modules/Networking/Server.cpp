#include "Server.h"
#include "Packet.h"
#include "../../utility/ConsoleLogging.h"

namespace ecse::Networking
{
	std::shared_ptr<Server> Server::Start(asio::ip::port_type port)
	{
		auto server = std::make_shared<Server>(port);
		server->Listen();
		server->_Thread = new std::thread([server]() { server->_Context.run(); });
		return server;
	}

	void Server::Stop()
	{
		_Context.stop();
	}

	void Server::Listen()
	{
		_Socket.async_receive_from(asio::buffer(_Buffer), _SenderEndpoint,
			std::bind(&Server::HandleReceive, this, std::placeholders::_1, std::placeholders::_2));
	}

	void Server::HandleReceive(const asio::error_code& error, size_t bytes_transferred)
	{
		if (error)
		{
			LogError(error.message(), false);
			return;
		}
		if (bytes_transferred < PacketHeader::HEADER_SIZE)
		{
			LogError("Received packet with invalid size " + std::to_string(bytes_transferred), false);
			return;
		}

		ReceivedPacket packet(_SenderEndpoint, _Buffer.data(), bytes_transferred);
		
		PacketHandler* handler;
		if (!TryGetPacketHandler(packet.packet.ID(), handler))
		{
			LogWarning("Received unknown packet id '" + std::to_string(packet.packet.ID()) + "' from " + _SenderEndpoint.address().to_string(), false);
			Listen();
			return;
		}
		
		(*handler)(&packet);

		std::cout << "Received " << bytes_transferred << " bytes from " << _SenderEndpoint << std::endl;
		Listen();
	}
}