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

void CreateScene(flecs::world& world)
{
	auto defaultShader = CCX::LoadShaderProgram("default_vs", "default_fs");
	auto paddleMesh = Thera::LoadMesh("Resources/Cube.obj");
	
	flecs::entity leftPaddle = world.entity("LeftPaddle");
	auto leftRenderer = leftPaddle.get_mut<Thera::Rendering::Renderer>();
	leftRenderer->meshes.push_back(paddleMesh);
	leftRenderer->material = defaultShader;
	leftPaddle.set<Thera::Core::Transform>({ glm::dvec3(-29, 0, 0), glm::identity<glm::quat>(), glm::vec3(1,4,1) });
	
	flecs::entity rightPaddle = world.entity("RightPaddle");
	auto rightRenderer = rightPaddle.get_mut<Thera::Rendering::Renderer>();
	rightRenderer->meshes.push_back(paddleMesh);
	rightRenderer->material = defaultShader;
	rightPaddle.set<Thera::Core::Transform>({ glm::dvec3(29, 0, 0), glm::identity<glm::quat>(), glm::vec3(1,4,1) });

	flecs::entity ball = world.entity("Ball");
	auto ballRenderer = ball.get_mut<Thera::Rendering::Renderer>();
	ballRenderer->meshes.push_back(paddleMesh);
	ballRenderer->material = defaultShader;
	ball.set<Thera::Core::Transform>({ glm::dvec3(0, 0, 0), glm::identity<glm::quat>(), glm::vec3(1,1,1) });

	float aspect = (float)Thera::WindowWidth / (float)Thera::WindowHeight;

	flecs::entity cam = world.entity();
	cam.set<Thera::Rendering::Camera>({ Thera::Rendering::CameraView::Orthographic, Thera::Core::MaskBehaviour::None, 0.1f, 100.0f, 1.0472f, glm::vec2(30, 30 / aspect)});
	cam.set<Thera::Core::Transform>({ glm::dvec3(0, 0, 10), glm::identity<glm::quat>(), glm::vec3(1,1,1) });
}

int main()
{
	Thera::Net::tcp::RegisterPacket(PacketType::Connect, HandleConnect);
	Thera::Net::tcp::RegisterPacket(PacketType::PlayerJoin, HandlePlayerJoin);
	Thera::Net::tcp::RegisterPacket(PacketType::PlayerLeave, HandlePlayerLeave);
	Thera::Net::tcp::RegisterPacket(PacketType::Ready, HandleReady);
	Thera::Net::tcp::RegisterPacket(PacketType::Start, HandleStart);
	Thera::Net::tcp::RegisterPacket(PacketType::PlayerPosition, HandlePlayerPosition);
	Thera::Net::tcp::RegisterPacket(PacketType::BallPosition, HandleBallPosition);
	Thera::Net::tcp::RegisterPacket(PacketType::Score, HandleScore);
	
	Thera::Init();

	auto world = Thera::GetWorld();
	CreateScene(*world);

	ImGui::StyleColorsDark();

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