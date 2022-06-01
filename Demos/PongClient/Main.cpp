#include <Modules/Networking/Networking.h>
#include <Modules/Networking/TCP/TCPNetworking.h>
#include <iostream>
#include <vector>

ecse::Networking::Connection* connection;

void HandleConnect(const asio::ip::udp::endpoint& source, ecse::Networking::Packet* packet)
{
	ecse::Networking::Packet pack(ecse::Networking::PacketType::Heartbeat);
	connection->Send(pack);
	connection->Flush();
}

void HandleConnectTCP(ecse::Networking::tcp::Connection& source, ecse::Networking::Packet* packet)
{
	char helloworld[13];
	*packet >> helloworld;
	LogInfo((std::ostringstream() << "TCP received: " << helloworld).str());
}

int main()
{
	ecse::Networking::RegisterPacket(ecse::Networking::PacketType::Connect, HandleConnect);
	ecse::Networking::tcp::RegisterPacket(ecse::Networking::PacketType::Connect, HandleConnectTCP);

	asio::ip::address_v4 target = asio::ip::make_address_v4("202.91.194.220");
	asio::ip::address_v4 targetLAN = asio::ip::make_address_v4("192.168.1.108");

	connection = new ecse::Networking::Connection(asio::ip::udp::endpoint(asio::ip::udp::v4(), 7331), target, 1337);
	
	ecse::Networking::Packet pack1(ecse::Networking::PacketType::Connect);
	pack1 << "Hello World!";
	connection->Send(pack1);
	connection->Flush();
	
	ecse::Networking::DiscoverMTU(target);

	auto tcpConnection = ecse::Networking::tcp::Connection(asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 7331), targetLAN, 1337);
	tcpConnection.Send(pack1);
	tcpConnection.Flush();

	system("pause");


	return 0;
}