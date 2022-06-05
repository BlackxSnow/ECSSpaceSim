#include <Engine.h>
#include <Modules/Networking/Networking.h>
#include <Modules/Networking/TCP/TCPNetworking.h>
#include <Modules/UI/UI.h>
#include <Modules/Input/Input.h>

#include <iostream>
#include <fstream>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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

	//ultralight::Config config;
	//auto& platform = ultralight::Platform::instance();
	//platform.set_config(config);

	//platform.set_font_loader(ultralight::GetPlatformFontLoader());
	//platform.set_file_system(ultralight::GetPlatformFileSystem("."));
	//platform.set_logger(ultralight::GetDefaultLogger("ultralight.log"));

	//auto renderer = ultralight::Renderer::Create();
	//ultralight::ViewConfig vConfig;
	//vConfig.is_transparent = true;
	//auto view = renderer->CreateView(Thera::WindowWidth, Thera::WindowHeight, vConfig, nullptr);
	//
	//std::ifstream htmlFile("./Resources/UI/Pong.html");
	//std::stringstream html;
	//html << htmlFile.rdbuf();
	//auto htmlstr = html.str();

	//view->LoadHTML(ultralight::String(htmlstr.c_str(), htmlstr.size()));
	////view->LoadURL("https://calvin.soueid.io/");
	//
	//view->Focus();
	//renderer->Update();
	//renderer->Render();
	//auto surface = view->surface();
	//auto bitmap = static_cast<ultralight::BitmapSurface*>(surface)->bitmap();

	//auto mem = bgfx::copy(bitmap->LockPixels(), bitmap->size());
	//auto tex = bgfx::createTexture2D(bitmap->width(), bitmap->height(), false, 1, bgfx::TextureFormat::BGRA8, 0);
	//bgfx::updateTexture2D(tex, 0, 0, 0, 0, bitmap->width(), bitmap->height(), mem);
	//bitmap->UnlockPixels();

	//Thera::Input::RawMouseInputReceived.Register([view](GLFWwindow* window, int button, int action, int mods) {
	//	auto ulMouse = Thera::UI::Input::TheraToUL(Thera::Input::GLFWInputToMouse(button));
	//	ultralight::MouseEvent mv{};
	//	mv.type = action == GLFW_PRESS ? ultralight::MouseEvent::kType_MouseDown : ultralight::MouseEvent::kType_MouseUp;
	//	mv.button = ulMouse;
	//	mv.x = Thera::Input::mousePosition.x;
	//	mv.y = Thera::Input::mousePosition.y;
	//	view->FireMouseEvent(mv);
	//});

	//Thera::OnFinalValidate.Register([view]() { 
	//	ultralight::MouseEvent mv{};
	//	mv.type = ultralight::MouseEvent::kType_MouseMoved;
	//	mv.x = Thera::Input::mousePosition.x;
	//	mv.y = Thera::Input::mousePosition.y;
	//	mv.button = ultralight::MouseEvent::Button::kButton_Left;
	//	view->FireMouseEvent(mv);
	//});

	//Thera::OnFinalValidate.Register([renderer, bitmap, tex, view]()
	//{
	//	renderer->Update();
	//	renderer->Render();
	//	auto bitmapMem = bgfx::copy(bitmap->LockPixels(), bitmap->size());
	//	bgfx::updateTexture2D(tex, 0, 0, 0, 0, bitmap->width(), bitmap->height(), bitmapMem);
	//	bitmap->UnlockPixels();
	//});

	flecs::entity cam = Thera::GetWorld()->entity();
	cam.set<Thera::Rendering::Camera>({ Thera::Rendering::CameraView::Perspective, Thera::Core::MaskBehaviour::None, 0.1f, 100.0f, 1.0472f });

	//flecs::entity texTest = Thera::GetWorld()->entity();
	//texTest.set<Thera::UI::ScreenRenderer>({ tex });

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