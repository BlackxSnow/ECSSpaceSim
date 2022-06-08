#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>



#include <Modules/Networking/Networking.h>
#include <Modules/Networking/TCP/TCPNetworking.h>

#include <vector>
#include <iostream>

enum class PacketType
{
	Connect,
	PlayerJoin,
	PlayerLeave,
	Ready,
	Start,
	PlayerPosition,
	BallPosition,
	Score
};

struct Client
{
	std::shared_ptr<Thera::Net::tcp::Connection> TCP;
	std::shared_ptr<Thera::Net::Connection> UDP;
	std::string Name;
};

std::unordered_map<asio::ip::tcp::endpoint, std::shared_ptr<Thera::Net::tcp::Connection>> _ConnectionRequests;
std::unordered_map<asio::ip::tcp::endpoint, Client> _ConnectedClients;

void HandleConnectTCP(Thera::Net::tcp::Connection& source, Thera::Net::Packet* packet)
{
	auto reader = packet->GetReader();
	auto name = reader.Read<std::string>();
	
	LogInfo((std::ostringstream() << "Client requested connection with name: " << name).str());
	
	Thera::Net::Packet connectResponse(PacketType::Connect);
	connectResponse.Write(static_cast<unsigned char>(_ConnectedClients.size()));
	if (_ConnectedClients.size() == 1)
	{
		connectResponse.Write<std::string>(_ConnectedClients.begin()->second.Name);
	}
	if (_ConnectedClients.size() < 2)
	{
		Client& client = _ConnectedClients[source.Endpoint()];
		client.Name = name;
		client.TCP = _ConnectionRequests.at(source.Endpoint());
		_ConnectionRequests.erase(source.Endpoint());
	}
	source.Send(connectResponse);
	source.Flush();
}

void OnDisconnect(std::shared_ptr<Thera::Net::tcp::Connection> connection)
{
	_ConnectedClients.erase(connection->Endpoint());
	_ConnectionRequests.erase(connection->Endpoint());
}

void TCPOnAccept(std::shared_ptr<Thera::Net::tcp::Connection> connection)
{
	LogInfo((std::ostringstream() << "Accepted TCP from: " << connection->Endpoint()).str());
	connection->Disconnected.Register(OnDisconnect);
	_ConnectionRequests[connection->Endpoint()] = connection;
	connection->Activate();
}

int main(int argc, char* argv[])
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	try
	{
		Thera::Net::tcp::RegisterPacket(Thera::Net::PacketType::Connect, HandleConnectTCP);
		
		auto udpListener = Thera::Net::Connection::Create(1337);
		auto tcpListener = Thera::Net::tcp::Listener::Create(1337, TCPOnAccept);

		while (true)
		{
			
		}
		
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	system("pause");
	return 0;
}