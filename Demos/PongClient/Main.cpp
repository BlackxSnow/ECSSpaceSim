#include <Modules/Networking/Networking.h>
#include <Modules/Networking/TCP/TCPNetworking.h>
#include <iostream>
#include <vector>

Thera::Net::Connection* connection;

void HandleConnect(const asio::ip::udp::endpoint& source, Thera::Net::Packet* packet)
{
	Thera::Net::Packet pack(Thera::Net::PacketType::Heartbeat);
	connection->Send(pack);
	connection->Flush();
}

void HandleConnectTCP(Thera::Net::tcp::Connection& source, Thera::Net::Packet* packet)
{
	char helloworld[13];
	*packet >> helloworld;
	LogInfo((std::ostringstream() << "TCP received: " << helloworld).str());
}

int main()
{
	Thera::Net::RegisterPacket(Thera::Net::PacketType::Connect, HandleConnect);
	Thera::Net::tcp::RegisterPacket(Thera::Net::PacketType::Connect, HandleConnectTCP);

	asio::ip::address_v4 target = asio::ip::make_address_v4("202.91.194.220");
	asio::ip::address_v4 targetLAN = asio::ip::make_address_v4("192.168.1.108");

	connection = new Thera::Net::Connection(asio::ip::udp::endpoint(asio::ip::udp::v4(), 7331), target, 1337);
	
	Thera::Net::Packet pack1(Thera::Net::PacketType::Connect);
	pack1 << "Hello World!";
	connection->Send(pack1);
	connection->Flush();
	
	Thera::Net::DiscoverMTU(target);

	auto tcpConnection = Thera::Net::tcp::Connection(asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 7331), targetLAN, 1337);
	tcpConnection.Send(pack1);
	tcpConnection.Flush();

	system("pause");


	return 0;
}