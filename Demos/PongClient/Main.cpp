#include <Modules/Networking/Networking.h>
#include <iostream>
#include <vector>

ecse::Networking::Connection* connection;

void HandleConnect(const asio::ip::udp::endpoint& source, ecse::Networking::Packet* packet)
{
	ecse::Networking::Packet pack(ecse::Networking::PacketType::Heartbeat);
	connection->Send(pack);
	connection->Flush();
}


int main()
{
	ecse::Networking::RegisterPacket(ecse::Networking::PacketType::Connect, HandleConnect);
	connection = new ecse::Networking::Connection(asio::ip::udp::endpoint(asio::ip::make_address("127.0.0.1"), 7331), asio::ip::make_address("127.0.0.1"), 1337);
	
	ecse::Networking::Packet pack1(ecse::Networking::PacketType::Connect);
	pack1 << "Hello World!";
	connection->Send(pack1);
	connection->Flush();
	
	ecse::Networking::Packet loopPack(ecse::Networking::PacketType::Heartbeat);

	int count = 8;
	while (true)
	{
		system("pause");
		for (int i = 0; i < count; i++)
		{
			connection->Send(loopPack);
		}
		LogInfo("Sending " + std::to_string(count) + " packets with size " + std::to_string(ecse::Networking::PacketHeader::HEADER_SIZE * count + ecse::Networking::AggregatePacket::HEADER_SIZE));
		connection->Flush();
		count *= 2;
	}


	return 0;
}