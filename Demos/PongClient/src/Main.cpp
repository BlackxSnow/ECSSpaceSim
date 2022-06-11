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
#include "GameScene.h"


void CreateInput()
{
	namespace inp = Thera::Input;

	inp::SetBindContext(inp::CreateAction("Move", inp::Output::Scalar, inp::Precision::Single));
	inp::CreateCompositeBinding(inp::Output::Scalar, inp::Precision::Single,
		{
			inp::CreateConstituent(inp::Key::Down, { inp::Component::NegX }),
			inp::CreateConstituent(inp::Key::Up, { inp::Component::PosX })
		});
}

const float PaddleSpeed = 20.0f;

void PerformMovement(flecs::entity entity, const Player& player, Thera::Core::Transform& transform)
{
	float input = Thera::Input::GetAction("Move")->GetData<float>();
	transform.Position.y += input * Thera::DeltaTime() * PaddleSpeed;
	if (input != 0)
	{
		Thera::Net::Packet position(PacketType::PlayerPosition);
		position.Write(static_cast<float>(transform.Position.y));
		CurrentServer.TCP->Send(position);
	}
}

void UpdateOpponentPosition(flecs::entity entity, Thera::Core::Transform& transform, const Opponent&)
{
	if (!CurrentGame) return;
	transform.Position.y = CurrentGame->OpponentPosition;
}

void UpdateBallPosition(flecs::entity entity, Thera::Core::Transform& transform, const PongBall&)
{
	if (!CurrentGame) return;
	glm::vec2& ballPos = CurrentGame->BallPosition;
	transform.Position = glm::dvec3(ballPos.x, ballPos.y, 0);
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
	CreateInput();

	world->system<const Player, Thera::Core::Transform>("PlayerMovement").kind(flecs::OnUpdate).each(PerformMovement);

	ImGui::StyleColorsDark();

	flecs::system buildUI = world->system("BuildUI").kind(flecs::OnRender).iter(BuildUI);
	world->system("FlushConnections").kind(flecs::PostFrame).iter([](flecs::iter& iter) { CurrentServer.Flush(); });

	world->system<Thera::Core::Transform, const Opponent>("UpdateOpponentPosition").kind(flecs::OnUpdate).each(UpdateOpponentPosition);
	world->system<Thera::Core::Transform, const PongBall>("UpdateBallPosition").kind(flecs::OnUpdate).each(UpdateBallPosition);

	Thera::Loop(0, nullptr);

	CurrentServer.Close();

	return 0;
}