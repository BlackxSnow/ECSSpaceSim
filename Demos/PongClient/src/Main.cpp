#include <Engine.h>
#include <Modules/Networking/Networking.h>
#include <Modules/Networking/TCP/TCPNetworking.h>
#include <Modules/UI/UI.h>
#include <Modules/Input/Input.h>

#include <iostream>
#include <fstream>
#include <vector>

#include "GameUI.h"
#include "GameState.h"

int main()
{
	Thera::Net::tcp::RegisterPacket(PacketType::Connect, HandleConnect);
	Thera::Net::tcp::RegisterPacket(PacketType::PlayerJoin, HandlePlayerJoin);
	Thera::Net::tcp::RegisterPacket(PacketType::PlayerLeave, HandlePlayerLeave);
	Thera::Net::tcp::RegisterPacket(PacketType::Ready, HandleReady);
	Thera::Net::tcp::RegisterPacket(PacketType::Start, HandleStart);
	Thera::Net::tcp::RegisterPacket(PacketType::Score, HandleScore);
	
	Thera::Init();

	auto world = Thera::GetWorld();

	ImGui::StyleColorsDark();

	flecs::entity cam = world->entity();
	cam.set<Thera::Rendering::Camera>({ Thera::Rendering::CameraView::Perspective, Thera::Core::MaskBehaviour::None, 0.1f, 100.0f, 1.0472f });

	flecs::system buildUI = world->system().kind(flecs::OnRender).iter(BuildUI);

	Thera::Loop(0, nullptr);

	CurrentServer.Close();
	
	//STARTUPINFO startupInfo;
	//PROCESS_INFORMATION processInfo;

	//std::memset(&startupInfo, 0, sizeof(startupInfo));
	//startupInfo.cb = sizeof(startupInfo);
	//std::memset(&processInfo, 0, sizeof(processInfo));

	//const LPCWSTR serverPath = L"../PongServer/bin/Win64/Debug/PongServer.exe";
	//bool server = CreateProcess(serverPath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo);

	//if (!server)
	//{
	//	LogError((std::ostringstream() << "Startup failed: " << GetLastError()).str(), false);
	//}

	//WaitForSingleObject(processInfo.hProcess, INFINITE);

	//CloseHandle(processInfo.hProcess);
	//CloseHandle(processInfo.hThread);

	return 0;
}