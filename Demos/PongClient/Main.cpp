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
	connection = new ecse::Networking::Connection(asio::ip::udp::endpoint(asio::ip::udp::v4(), 7331), asio::ip::make_address("202.91.194.220"), 1337);
	
	ecse::Networking::Packet pack1(ecse::Networking::PacketType::Connect);
	pack1 << "Hello World!";
	connection->Send(pack1);
	connection->Flush();
	
	ecse::Networking::DiscoverMTU(202,91,194,220);
	system("pause");


	ecse::Networking::Packet loopPack(ecse::Networking::PacketType::Heartbeat);
	char someData[256];

	while (true)
	{
		//system("pause");
		//loopPack << someData;
		//LogInfo("Sending " + std::to_string(loopPack.FullSize()) + " bytes");
		//connection->Send(loopPack);
		//connection->Flush();
	}


	return 0;
}