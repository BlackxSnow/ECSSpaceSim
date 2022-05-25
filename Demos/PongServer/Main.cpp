#include <Modules/Networking/Networking.h>

#include <vector>
#include <iostream>

std::vector<char> receiveBuffer(512);

void Listen();

void HandleReceive(const asio::error_code& error, size_t transferred)
{
	std::cout << "Received " << transferred << " bytes from direct socket" << std::endl;
	Listen();
}


asio::ip::udp::endpoint* _endpoint;
asio::ip::udp::socket* _socket;

void Listen()
{
	_socket->async_receive_from(asio::buffer(receiveBuffer), *_endpoint, HandleReceive);
}

void HandleConnect(ecse::Networking::ReceivedPacket* packet)
{
	char helloworld[13];
	packet->packet >> helloworld;
	std::cout << "Received: " << helloworld << std::endl;
}

int main()
{
	try
	{
		ecse::Networking::RegisterPacket(ecse::Networking::PacketType::Connect, HandleConnect);
		auto server = ecse::Networking::Server::Start(1337);
		
		//asio::io_context context;
		//asio::ip::udp::resolver resolver(context);
		//_endpoint = new asio::ip::udp::endpoint(asio::ip::make_address("127.0.0.1"), 1337);
		//_socket = new asio::ip::udp::socket(context);

		//_socket->open(asio::ip::udp::v4());
		//_socket->set_option(asio::ip::udp::socket::reuse_address(true));
		//_socket->bind(*_endpoint);
		//Listen();

		//std::thread thread ([&]()
		//{
		//	context.run();
		//});

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