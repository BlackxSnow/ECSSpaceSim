#include <Modules/Networking/Networking.h>
#include <Modules/Networking/TCP/TCPNetworking.h>

#include <vector>
#include <iostream>

enum class PacketType
{
	Connect
};

std::list<Thera::Net::Connection> _Clients;
std::list<std::shared_ptr<Thera::Net::tcp::Connection>> _TCPClients;

std::string motd;

void HandleConnectTCP(Thera::Net::tcp::Connection& source, Thera::Net::Packet* packet)
{
	auto reader = packet->GetReader();
	auto name = reader.Read<std::string>();
	
	LogInfo("Client connected! Name: " + name);
	
	Thera::Net::Packet motdResponse(Thera::Net::PacketType::Connect);
	motdResponse.Write<std::string>(motd);
	source.Send(motdResponse);
	source.Flush();
}

void TCPOnAccept(std::shared_ptr<Thera::Net::tcp::Connection> connection)
{
	LogInfo((std::ostringstream() << "Accepted TCP from: " << connection->Endpoint()).str());
	_TCPClients.push_back(connection);
}

int main()
{
	try
	{
		std::cout << "Enter server MOTD" << std::endl;
		std::getline(std::cin, motd);

		Thera::Net::tcp::RegisterPacket(Thera::Net::PacketType::Connect, HandleConnectTCP);
		
		Thera::Net::Connection udpListener(1337);
		Thera::Net::tcp::Listener tcpListener(1337, TCPOnAccept);

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