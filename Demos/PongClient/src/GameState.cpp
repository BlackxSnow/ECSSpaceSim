#include "GameState.h"
#include "GameUI.h"


void Connect(const asio::ip::address_v4& ip, const asio::ip::port_type port, const std::string& playerName)
{
	asio::ip::tcp::endpoint localTCP(asio::ip::tcp::v4(), 0);
	asio::ip::udp::endpoint localUDP(asio::ip::udp::v4(), 0);
	asio::error_code error;
	auto tcpConnection = Thera::Net::tcp::Connection::Create(localTCP, ip, port, error);
	if (error)
	{
		LogError((std::ostringstream() << "Error during connection: " << error.message()).str(), false);
		CurrentState = GameState::Join;
		return;
	}
	LogInfo((std::ostringstream() << "TCP connected from " << tcpConnection->LocalEndpoint()).str());
	auto udpConnection = std::make_shared<Thera::Net::Connection>(localUDP, ip, port);

	Thera::Net::Packet connect(PacketType::Connect);
	connect.Write(playerName);
	tcpConnection->Send(connect);
	tcpConnection->Flush();

	CurrentServer.TCP = tcpConnection;
	CurrentServer.UDP = udpConnection;
}

void HandlePlayerJoin(Thera::Net::tcp::Connection& source, Thera::Net::Packet* packet)
{
	auto reader = packet->GetReader();
	std::string opponent = reader.Read < std::string>();
	LogInfo((std::ostringstream() << "Player joined as " << opponent).str());
	CurrentGame->OpponentName = opponent;
}

void HandlePlayerLeave(Thera::Net::tcp::Connection& source, Thera::Net::Packet* packet)
{
	LogInfo((std::ostringstream() << "Player " << CurrentGame->OpponentName << " disconnected.").str());
	CurrentGame->OpponentName = "";
	CurrentGame->OpponentReady = false;
	CurrentState = GameState::Lobby;
}

void HandleReady(Thera::Net::tcp::Connection& source, Thera::Net::Packet* packet)
{
	auto reader = packet->GetReader();
	bool ready = reader.Read<bool>();
	CurrentGame->OpponentReady = ready;
}

void HandleStart(Thera::Net::tcp::Connection& source, Thera::Net::Packet* packet)
{
	CurrentGame->OpponentReady = false;
	CurrentGame->SelfReady = false;
	CurrentGame->OpponentScore = 0;
	CurrentGame->SelfScore = 0;
	CurrentState = GameState::Game;
}

void HandleScore(Thera::Net::tcp::Connection& source, Thera::Net::Packet* packet)
{
}

void SetupLobby(std::string opponent = "")
{
	CurrentState = GameState::Lobby;
	if (CurrentGame)
	{
		delete CurrentGame;
		LogWarning("Last game data was erroneously not cleaned up.");
	}
	byte side = opponent.empty() ? 0 : 1;
	CurrentGame = new GameInfo{side, PlayerName, opponent, 0, 0, false, false };
}

void HandleConnect(Thera::Net::tcp::Connection& source, Thera::Net::Packet* packet)
{
	auto reader = packet->GetReader();
	byte playerCount = reader.Read<byte>();
	switch (playerCount)
	{
	case 0:
		LogInfo("Successfully connected to empty server.");
		SetupLobby();
		break;
	case 1:
	{
		std::string otherPlayer = reader.Read<std::string>();
		LogInfo((std::ostringstream() << "Successfully connected to server. Other player: " << otherPlayer).str());
		SetupLobby(otherPlayer);
		break;
	}
	case 2:
		LogError("Error while connecting: Server already at capacity.", false);
		CurrentState = GameState::Join;
		break;
	default:
		LogError("Recieved malformed connect response packet from server.", false);
		CurrentState = GameState::Join;
		break;
	}
}