#pragma once
#include <modules/networking/Networking.h>
#include <modules/networking/tcp/TCPNetworking.h>

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
	Score
};

struct GameInfo
{
	byte SelfSide; // 0 = left, 1 = right
	std::string SelfName;
	std::string OpponentName;
	byte SelfScore;
	byte OpponentScore;
	bool SelfReady;
	bool OpponentReady;
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
		TCP.reset();
		UDP.reset();
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
void HandleScore(Thera::Net::tcp::Connection& source, Thera::Net::Packet* packet);