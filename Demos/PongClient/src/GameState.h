#pragma once
#include <modules/networking/Networking.h>
#include <modules/networking/tcp/TCPNetworking.h>
#include <glm/vec2.hpp>

enum class GameState
{
	Menu,
	Loading,
	Host,
	Join,
	Lobby,
	Game
};

enum class PacketType
{
	Connect,
	PlayerJoin,
	PlayerLeave,
	Ready,
	Start,
	PlayerPosition,
	BallPosition,
	Score
};

struct GameInfo
{
	byte SelfSide; // 0 = left, 1 = right
	std::string SelfName;
	std::string OpponentName;
	byte LeftScore;
	byte RightScore;
	bool SelfReady;
	bool OpponentReady;
	
	float OpponentPosition;
	glm::vec2 BallPosition;
};

struct ServerConnection
{
	std::shared_ptr<Thera::Net::tcp::Connection> TCP;
	std::shared_ptr<Thera::Net::Connection> UDP;

	void Close()
	{
		if (TCP)
		{
			TCP->Close();
		}
		if (UDP)
		{
			UDP->Close();
		}
		TCP.reset();
		UDP.reset();
	}

	void Flush()
	{
		if (TCP)
		{
			TCP->Flush();
		}
		if (UDP)
		{
			UDP->Flush();
		}
	}
};

inline GameState CurrentState;
inline GameInfo* CurrentGame;
inline ServerConnection CurrentServer;


void Connect(const asio::ip::address_v4& ip, const asio::ip::port_type port, const std::string& playerName);

void HandleConnect(Thera::Net::tcp::Connection& source, Thera::Net::Packet* packet);
void HandlePlayerJoin(Thera::Net::tcp::Connection& source, Thera::Net::Packet* packet);
void HandlePlayerLeave(Thera::Net::tcp::Connection& source, Thera::Net::Packet* packet);
void HandleReady(Thera::Net::tcp::Connection& source, Thera::Net::Packet* packet);
void HandleStart(Thera::Net::tcp::Connection& source, Thera::Net::Packet* packet);
void HandlePlayerPosition(Thera::Net::tcp::Connection& source, Thera::Net::Packet* packet);
void HandleBallPosition(Thera::Net::tcp::Connection& source, Thera::Net::Packet* packet);
void HandleScore(Thera::Net::tcp::Connection& source, Thera::Net::Packet* packet);