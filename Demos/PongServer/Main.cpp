#include <Modules/Networking/Networking.h>

#include <vector>
#include <iostream>

std::list<ecse::Networking::Connection> _Clients;

void HandleConnect(const asio::ip::udp::endpoint& source, ecse::Networking::Packet* packet)
{
	char helloworld[13];
	*packet >> helloworld;
	std::cout << "Received: " << helloworld << std::endl;
	auto& con = _Clients.emplace_back(asio::ip::udp::endpoint(asio::ip::make_address("127.0.0.1"), 1337), source);
	con.Send(ecse::Networking::Packet(ecse::Networking::PacketType::Connect));
	con.Flush();
}

int main()
{
	try
	{
		ecse::Networking::RegisterPacket(ecse::Networking::PacketType::Connect, HandleConnect);
		auto server = ecse::Networking::Connection(1337);

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