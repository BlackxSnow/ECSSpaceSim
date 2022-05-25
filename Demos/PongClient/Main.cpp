#include <Modules/Networking/Networking.h>
#include <iostream>
#include <vector>


int main()
{
	asio::io_context context;

	asio::ip::udp::resolver resolver(context);
	asio::ip::udp::endpoint local(asio::ip::make_address("127.0.0.1"), 7331);
	asio::ip::udp::endpoint destination(asio::ip::make_address("127.0.0.1"), 1337);

	asio::ip::udp::socket socket(context);
	socket.open(asio::ip::udp::v4());
	socket.bind(local);

	ecse::Networking::Packet packet(ecse::Networking::PacketType::Connect);
	packet << "Hello World!";

	//auto buffer = asio::buffer(asio::buffer(&packet, ecse::Networking::PacketHeader::HEADER_SIZE), asio::buffer(packet.)

	//socket.send_to(asio::buffer(&packet, packet.FullSize()), destination);
	socket.send_to(packet.GetBuffer(), destination);


	return 0;
}