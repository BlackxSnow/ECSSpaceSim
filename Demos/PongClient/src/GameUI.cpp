#include "GameUI.h"
#include "GameState.h"
#include <misc/cpp/imgui_stdlib.h>
#include <utility/ConsoleLogging.h>

#include <asio.hpp>

void BuildMenu()
{
	imgui::SetNextWindowPos(ImVec2(Thera::WindowWidth / 2.0f, Thera::WindowHeight / 2.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	imgui::Begin("Menu", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

	/*if (imgui::Button("Host & Play", ImVec2(200, 75)))
	{
		CurrentState = GameState::Host;
	}*/
	if (imgui::Button("Join Game", ImVec2(200, 75)))
	{
		CurrentState = GameState::Join;
	}
	if (imgui::Button("Exit", ImVec2(200, 75)))
	{
		for (GLFWwindow* window : Thera::GetWindowsMutable())
		{
			glfwSetWindowShouldClose(window, 1);
		}
	}
	imgui::End();
}

std::string _IPString = "";
int _Port = 1337;

void BuildJoin()
{
	imgui::SetNextWindowPos(ImVec2(Thera::WindowWidth / 2.0f, Thera::WindowHeight / 2.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	imgui::Begin("Join", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
	imgui::PushItemWidth(200);
	imgui::InputText("Name", &PlayerName);
	imgui::InputText("IP", &_IPString);
	imgui::InputInt("Port", &_Port);

	ImVec2 buttonSize = ImVec2(imgui::GetContentRegionAvail().x, 75);

	if (imgui::Button("Connect", buttonSize))
	{
		asio::error_code asioError;
		asio::ip::address_v4 ip = asio::ip::make_address_v4(_IPString, asioError);
		bool error = false;
		if (_Port < 0 || _Port > 65535)
		{
			LogError("Port was outside of valid range.", false);
			error = true;
		}
		if (asioError)
		{
			LogError("IP address was invalid.", false);
			error = true;
		}
		if (PlayerName.empty() || PlayerName.length() > 15)
		{
			LogError("Name must be at least 1 character and at most 15.", false);
			error = true;
		}
		if (!error)
		{
			CurrentState = GameState::Loading;
			Connect(ip, _Port, PlayerName);
		}
	}
	if (imgui::Button("Back", buttonSize))
	{
		CurrentState = GameState::Menu;
	}
	imgui::End();
}

void BuildLoad()
{
	imgui::SetNextWindowPos(ImVec2(Thera::WindowWidth / 2.0f, Thera::WindowHeight / 2.0f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	imgui::Begin("Loading", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs);
	imgui::Text("Loading...");
	imgui::End();
}

void BuildLobby()
{
	imgui::SetNextWindowPos(ImVec2(0, 20));
	imgui::Begin("Left Player Name", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar);
	imgui::Text("(%s)", CurrentGame->SelfSide == 0 ? CurrentGame->SelfName.c_str() : CurrentGame->OpponentName.c_str());
	imgui::End();

	const float scoreOffset = 50;

	imgui::SetNextWindowPos(ImVec2(static_cast<float>(Thera::WindowWidth) / 2.0f - scoreOffset, 20), 0, ImVec2(1,0));
	imgui::Begin("Left Player Score", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar);
	imgui::Text("(%s)", std::to_string(CurrentGame->LeftScore).c_str());
	imgui::End();

	imgui::SetNextWindowPos(ImVec2(static_cast<float>(Thera::WindowWidth) / 2.0f + scoreOffset, 20), 0, ImVec2(0, 0));
	imgui::Begin("Right Player Score", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar);
	imgui::Text("(%s)", std::to_string(CurrentGame->RightScore).c_str());
	imgui::End();

	imgui::SetNextWindowPos(ImVec2(static_cast<float>(Thera::WindowWidth), 20), 0, ImVec2(1,0));
	imgui::Begin("Right Player Name", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar);
	imgui::Text("(%s)", CurrentGame->SelfSide == 1 ? CurrentGame->SelfName.c_str() : CurrentGame->OpponentName.c_str());
	imgui::End();
}

void BuildUI(flecs::iter& iter)
{
	switch (CurrentState)
	{
	case GameState::Loading:
		BuildLoad();
		break;
	case GameState::Menu:
		BuildMenu();
		break;
	case GameState::Host:
		throw std::exception();
		/*BuildHost();*/
		break;
	case GameState::Join:
		BuildJoin();
		break;
	case GameState::Lobby:
		BuildLobby();
		break;
	case GameState::Game:
		break;
	default:
		break;
	}
}
