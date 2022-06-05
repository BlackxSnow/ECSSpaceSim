#include <Engine.h>
#include <Modules/Networking/Networking.h>
#include <Modules/Networking/TCP/TCPNetworking.h>
#include <Modules/UI/UI.h>
#include <Modules/Input/Input.h>

#include <iostream>
#include <fstream>
#include <vector>

#include <imgui.h>

enum class PacketType
{
	Connect
};

void HandleConnect(Thera::Net::tcp::Connection& source, Thera::Net::Packet* packet)
{
	auto reader = packet->GetReader();
	std::string motd = reader.Read<std::string>();
	LogInfo("Connected to server! MOTD: " + motd);
}

int main()
{
	Thera::Net::tcp::RegisterPacket(PacketType::Connect, HandleConnect);
	
	Thera::Init();

	auto world = Thera::GetWorld();

	ImGui::StyleColorsDark();

	flecs::entity cam = world->entity();
	cam.set<Thera::Rendering::Camera>({ Thera::Rendering::CameraView::Perspective, Thera::Core::MaskBehaviour::None, 0.1f, 100.0f, 1.0472f });

	world->system().kind(flecs::OnRender).iter([](flecs::iter& iter) {
		ImGui::Begin("Win1");

		ImGui::Text("Test Text");
		ImGui::End();
	});

	Thera::Loop(0, nullptr);

	//std::cout << "Enter your name" << std::endl;
	//std::string name;
	//std::getline(std::cin, name);
	//
	//std::cout << "Enter server IP" << std::endl;
	//std::string ipInput;
	//asio::error_code error;
	//asio::ip::address_v4 ip;
	//bool isValid = false;
	//while (!isValid)
	//{
	//	std::getline(std::cin, ipInput);
	//	ip = asio::ip::make_address_v4(ipInput, error);
	//	if (error)
	//	{
	//		std::cout << "Invalid IP, enter server IP" << std::endl;
	//	}
	//	else
	//	{
	//		isValid = true;
	//	}
	//}

	//std::cout << "Enter server port" << std::endl;
	//std::string portInput;
	//asio::ip::port_type port;
	//isValid = false;
	//while (!isValid)
	//{
	//	bool error = false;
	//	std::getline(std::cin, portInput);
	//	try
	//	{
	//		port = std::stoi(portInput);
	//	}
	//	catch (const std::exception&)
	//	{
	//		error = true;
	//	}
	//	if (port < 0 || port > 65535) error = true;
	//	if (error)
	//	{
	//		std::cout << "Enter server port as number between 0 and 65535" << std::endl;
	//		continue;
	//	}

	//	isValid = true;
	//}

	//asio::ip::tcp::endpoint localTCP = asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 7331);
	//asio::ip::udp::endpoint localUDP = asio::ip::udp::endpoint(asio::ip::udp::v4(), 7331);
	//auto tcpConnection = Thera::Net::tcp::Connection(localTCP, ip, port);
	//auto udpConnection = Thera::Net::Connection(localUDP, ip, port);

	//Thera::Net::Packet connect(PacketType::Connect);
	//connect.Write(name);
	//tcpConnection.Send(connect);
	//tcpConnection.Flush();

	//std::getchar();

	return 0;
}