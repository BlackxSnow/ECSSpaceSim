#include <Modules/Networking/Networking.h>
#include <Modules/Networking/TCP/TCPNetworking.h>

#include <vector>
#include <iostream>

std::list<ecse::Networking::Connection> _Clients;
std::list<std::shared_ptr<ecse::Networking::tcp::Connection>> _TCPClients;

void HandleConnect(const asio::ip::udp::endpoint& source, ecse::Networking::Packet* packet)
{
	char helloworld[13];
	*packet >> helloworld;
	std::cout << "Received: " << helloworld << std::endl;
	auto& con = _Clients.emplace_back(asio::ip::udp::endpoint(asio::ip::make_address("192.168.1.108"), 1337), source);
	con.Send(ecse::Networking::Packet(ecse::Networking::PacketType::Connect));
	con.Flush();
}

void HandleConnectTCP(ecse::Networking::tcp::Connection& source, ecse::Networking::Packet* packet)
{
	char helloworld[13];
	*packet >> helloworld;
	LogInfo((std::ostringstream() << "TCP received: " << helloworld).str());
	
	ecse::Networking::Packet pack(ecse::Networking::PacketType::Connect);
	pack << "Hello World!";
	source.Send(pack);
	source.Flush();
}

void TCPOnAccept(std::shared_ptr<ecse::Networking::tcp::Connection> connection)
{
	LogInfo((std::ostringstream() << "Accepted TCP from: " << connection->Endpoint()).str());
	_TCPClients.push_back(connection);
}

void test(int a) {};

int main()
{
	try
	{
		ecse::Networking::RegisterPacket(ecse::Networking::PacketType::Connect, HandleConnect);
		ecse::Networking::tcp::RegisterPacket(ecse::Networking::PacketType::Connect, HandleConnectTCP);
		auto server = ecse::Networking::Connection(1337);
		ecse::Networking::tcp::Listener tcpListener(1337, TCPOnAccept);

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