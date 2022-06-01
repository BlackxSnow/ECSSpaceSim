#include <Modules/Networking/Networking.h>
#include <Modules/Networking/TCP/TCPNetworking.h>

#include <vector>
#include <iostream>

std::list<Thera::Net::Connection> _Clients;
std::list<std::shared_ptr<Thera::Net::tcp::Connection>> _TCPClients;

void HandleConnect(const asio::ip::udp::endpoint& source, Thera::Net::Packet* packet)
{
	char helloworld[13];
	*packet >> helloworld;
	std::cout << "Received: " << helloworld << std::endl;
	auto& con = _Clients.emplace_back(asio::ip::udp::endpoint(asio::ip::make_address("192.168.1.108"), 1337), source);
	con.Send(Thera::Net::Packet(Thera::Net::PacketType::Connect));
	con.Flush();
}

void HandleConnectTCP(Thera::Net::tcp::Connection& source, Thera::Net::Packet* packet)
{
	char helloworld[13];
	*packet >> helloworld;
	LogInfo((std::ostringstream() << "TCP received: " << helloworld).str());
	
	Thera::Net::Packet pack(Thera::Net::PacketType::Connect);
	pack << "Hello World!";
	source.Send(pack);
	source.Flush();
}

void TCPOnAccept(std::shared_ptr<Thera::Net::tcp::Connection> connection)
{
	LogInfo((std::ostringstream() << "Accepted TCP from: " << connection->Endpoint()).str());
	_TCPClients.push_back(connection);
}

void test(int a) {};

int main()
{
	try
	{
		Thera::Net::RegisterPacket(Thera::Net::PacketType::Connect, HandleConnect);
		Thera::Net::tcp::RegisterPacket(Thera::Net::PacketType::Connect, HandleConnectTCP);
		auto server = Thera::Net::Connection(1337);
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