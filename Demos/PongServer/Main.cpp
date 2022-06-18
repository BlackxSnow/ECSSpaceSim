#include <Engine.h>
#include <modules/networking/Networking.h>
#include <modules/networking/tcp/TCPNetworking.h>
#include <modules/core/Core.h>

#include <vector>
#include <iostream>

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

struct Client
{
	std::shared_ptr<Thera::Net::tcp::Connection> TCP;
	std::shared_ptr<Thera::Net::Connection> UDP;
	std::string Name;
	byte Side;
};

std::unordered_map<asio::ip::tcp::endpoint, std::shared_ptr<Thera::Net::tcp::Connection>> _ConnectionRequests;
std::unordered_map<asio::ip::tcp::endpoint, Client> _ConnectedClients;

flecs::entity LeftPaddle;
flecs::entity RightPaddle;
flecs::entity Ball;

float LeftPosition = 0;
float RightPosition = 0;

const double BallStartSpeed = 15;
const double BallSpeed = 40;

struct Left {};
struct Right {};

struct AABB
{
	glm::dvec3 Extents;
};

struct BallRB
{
	glm::dvec3 Velocity{ 0,0,0 };
};

struct Hit
{
	glm::dvec3 Normal{0,0,0};
};

bool AABB2AABB(Thera::Core::Transform& aTransform, Thera::Core::Transform& bTransform, AABB& aCollider, AABB& bCollider, OUT Hit& collision)
{
	double dx = aTransform.Position.x - bTransform.Position.x;
	double dy = aTransform.Position.y - bTransform.Position.y;
	double xPenetration = aCollider.Extents.x + bCollider.Extents.x - abs(dx);
	double yPenetration = aCollider.Extents.y + bCollider.Extents.y - abs(dy);
	
	if (xPenetration <= 0 || yPenetration <= 0)
	{
		return false;
	}

	if (xPenetration < yPenetration)
	{
		collision.Normal.x = std::copysign(1, dx);
	}
	else
	{
		collision.Normal.y = std::copysign(1, dy);
	}
	return true;
}

void CheckBallCollision(flecs::iter& iter, Thera::Core::Transform* paddleTransform, AABB* paddleCollider, Thera::Core::Transform& ballTransform, AABB& ballCollider, BallRB& ball)
{
	for (auto& i : iter)
	{
		Thera::Core::Transform& currentPaddleTransform = paddleTransform[i];
		AABB& currentPaddleCollider = paddleCollider[i];
		
		glm::dvec3 paddleMin = currentPaddleTransform.Position - currentPaddleCollider.Extents;
		glm::dvec3 paddleMax = currentPaddleTransform.Position + currentPaddleCollider.Extents;

		glm::dvec3 ballMin = ballTransform.Position - ballCollider.Extents;
		glm::dvec3 ballMax = ballTransform.Position + ballCollider.Extents;

		Hit collision;
		if (AABB2AABB(currentPaddleTransform, ballTransform, currentPaddleCollider, ballCollider, collision))
		{
			if (abs(collision.Normal.x) < abs(collision.Normal.y))
			{
				ball.Velocity = glm::reflect(ball.Velocity, collision.Normal);
				//LogInfo((std::ostringstream() << "Reflecting with normal " << collision.Normal.x << "," << collision.Normal.y).str());
				continue;
			}
			//LogInfo((std::ostringstream() << "Bouncing with normal " << collision.Normal.x << "," << collision.Normal.y).str());
			auto toBall = glm::normalize(ballTransform.Position - currentPaddleTransform.Position);
			auto toBallAdjusted = glm::normalize(toBall - glm::dvec3(collision.Normal.x, 0, 0));
			ball.Velocity = toBallAdjusted * BallSpeed;
		}
	}
}

flecs::query<Thera::Core::Transform, AABB> collisionQuery;

const double _PhysicsStep = 0.0166666;
double _AccumulatedPhysicsTime = 0;

void DoBallPhysics(flecs::entity entity, Thera::Core::Transform& ballTransform, AABB& ballAABB, BallRB& ball)
{
	_AccumulatedPhysicsTime += Thera::DeltaTime();
	while (_AccumulatedPhysicsTime > _PhysicsStep)
	{
		ballTransform.Position += ball.Velocity * _PhysicsStep;
		collisionQuery.iter([&ballTransform, &ballAABB, &ball](flecs::iter& iter, Thera::Core::Transform* paddleTransform, AABB* paddleAABB)
		{
			CheckBallCollision(iter, paddleTransform, paddleAABB, ballTransform, ballAABB, ball);
		});
		_AccumulatedPhysicsTime -= _PhysicsStep;
	}
}

void CheckForScore(flecs::entity entity, Thera::Core::Transform& ballTransform, BallRB& ball)
{
	if (abs(ballTransform.Position.x) > 29)
	{
		auto sign = std::copysign(1, ballTransform.Position.x);
		byte scoringSide = sign < 0 ? 1 : 0;
		ballTransform.Position = { 0,0,0 };
		ball.Velocity = { sign * BallStartSpeed, 0, 0 };

		Thera::Net::Packet score(PacketType::Score);
		score.Write(scoringSide);
		
		for (auto& [endpoint, client] : _ConnectedClients)
		{
			client.TCP->Send(score);
		}
	}
}

void CreateGameScene(flecs::world& world)
{
	LeftPaddle = world.entity("LeftPaddle");
	LeftPaddle.set<Thera::Core::Transform>({ glm::dvec3(-29, 0, 0), glm::identity<glm::quat>(), glm::vec3(1,4,1) });
	LeftPaddle.set<AABB>({ {1,4,1} });
	LeftPaddle.add<Left>();

	RightPaddle = world.entity("RightPaddle");
	RightPaddle.set<Thera::Core::Transform>({ glm::dvec3(29, 0, 0), glm::identity<glm::quat>(), glm::vec3(1,4,1) });
	RightPaddle.set<AABB>({ {1,4,1} });
	RightPaddle.add<Right>();

	Ball = world.entity("Ball");
	Ball.set<Thera::Core::Transform>({ glm::dvec3(0, 0, 0), glm::identity<glm::quat>(), glm::vec3(1,1,1) });
	Ball.set<AABB>({ {1,1,1} });

	float borderPos = 22.5f;

	auto topBorder = world.entity("TopBorder");
	topBorder.set<Thera::Core::Transform>({ glm::dvec3(0, borderPos, 0), glm::identity<glm::quat>(), glm::vec3(60, 1, 1) });
	topBorder.set<AABB>({ {60,1,1} });

	auto bottomBorder = world.entity("BottomBorder");
	bottomBorder.set<Thera::Core::Transform>({ glm::dvec3(0, -borderPos, 0), glm::identity<glm::quat>(), glm::vec3(60, 1, 1) });
	bottomBorder.set<AABB>({ {60,1,1} });
}

void HandleConnectTCP(Thera::Net::tcp::Connection& source, Thera::Net::Packet* packet)
{
	auto reader = packet->GetReader();
	auto name = reader.Read<std::string>();
	
	LogInfo((std::ostringstream() << "Client requested connection with name: " << name).str());
	
	Thera::Net::Packet connectResponse(PacketType::Connect);
	connectResponse.Write(static_cast<unsigned char>(_ConnectedClients.size()));
	byte side = 0;
	if (_ConnectedClients.size() == 1)
	{
		Client& existingClient = _ConnectedClients.begin()->second;
		existingClient.Side == 0 ? side = 1 : side = 0;
		connectResponse.Write<std::string>(existingClient.Name);

		Thera::Net::Packet playerJoin(PacketType::PlayerJoin);
		playerJoin.Write<std::string>(name);
		existingClient.TCP->Send(playerJoin);

		Thera::Defer([]() { Ball.set<BallRB>({ {BallStartSpeed,0,0} }); });
	}
	if (_ConnectedClients.size() < 2)
	{
		Client& client = _ConnectedClients[source.Endpoint()];
		client.Name = name;
		client.TCP = _ConnectionRequests.at(source.Endpoint());
		client.Side = side;
		_ConnectionRequests.erase(source.Endpoint());
	}
	source.Send(connectResponse);
}

void HandlePlayerPosition(Thera::Net::tcp::Connection& source, Thera::Net::Packet* packet)
{
	auto reader = packet->GetReader();
	float position = reader.Read<float>();
	Client& sourceClient = _ConnectedClients.at(source.Endpoint());
	byte side = sourceClient.Side;
	if (side == 0)
	{
		LeftPosition = position;
	}
	else
	{
		RightPosition = position;
	}

	Thera::Net::Packet playerPosition(PacketType::PlayerPosition);
	playerPosition.Write<float>(position);

	for (auto& [endpoint, client] : _ConnectedClients)
	{
		if (&client == &sourceClient) continue;
		client.TCP->Send(playerPosition);
	}
}

void OnDisconnect(std::shared_ptr<Thera::Net::tcp::Connection> connection)
{
	_ConnectedClients.erase(connection->Endpoint());
	_ConnectionRequests.erase(connection->Endpoint());
}

void TCPOnAccept(std::shared_ptr<Thera::Net::tcp::Connection> connection)
{
	LogInfo((std::ostringstream() << "Accepted TCP from: " << connection->Endpoint()).str());
	connection->Disconnected.Register(OnDisconnect);
	_ConnectionRequests[connection->Endpoint()] = connection;
	connection->Activate();
}

void SendBallPosition(flecs::entity entity, Thera::Core::Transform& transform, BallRB& ball)
{
	Thera::Net::Packet ballPosition(PacketType::BallPosition);
	ballPosition.Write<float>(transform.Position.x);
	ballPosition.Write<float>(transform.Position.y);
	for (auto& [endpoint, client] : _ConnectedClients)
	{
		client.TCP->Send(ballPosition);
	}
}

void SetLeftPaddlePosition(flecs::entity entity, Thera::Core::Transform& transform, const Left& right)
{
	transform.Position.y = LeftPosition;
}
void SetRightPaddlePosition(flecs::entity entity, Thera::Core::Transform& transform, const Right& right)
{
	transform.Position.y = RightPosition;
}

void FlushConnections(flecs::iter&)
{
	for (auto& [endpoint, client] : _ConnectedClients)
	{
		if (client.TCP) client.TCP->Flush();
		if (client.UDP) client.UDP->Flush();
	}
}

void CleanupConnections()
{
	for (auto& [endpoint, connection] : _ConnectedClients)
	{
		if (connection.TCP) connection.TCP->Close();
		if (connection.UDP) connection.UDP->Close();
	}
}

int main(int argc, char* argv[])
{
	try
	{
		Thera::Net::tcp::RegisterPacket(Thera::Net::PacketType::Connect, HandleConnectTCP);
		Thera::Net::tcp::RegisterPacket(PacketType::PlayerPosition, HandlePlayerPosition);
		
		Thera::InitWindowless();

		auto world = Thera::GetWorld();
		CreateGameScene(*world);

		world->system<Thera::Core::Transform, const Left>("SetLeftPos").kind(flecs::OnUpdate).each(SetLeftPaddlePosition);
		world->system<Thera::Core::Transform, const Right>("SetRightPos").kind(flecs::OnUpdate).each(SetRightPaddlePosition);

		world->system("FlushConnections").kind(flecs::PostFrame).iter(FlushConnections);

		collisionQuery = world->query_builder<Thera::Core::Transform, AABB>().term<BallRB>().oper(flecs::Not).build();
		world->system<Thera::Core::Transform, AABB, BallRB>("DoBallPhysics").each(DoBallPhysics);
		world->system<Thera::Core::Transform, BallRB>("SendBallPosition").each(SendBallPosition);
		world->system<Thera::Core::Transform, BallRB>("CheckForScore").each(CheckForScore);

		//auto udpListener = Thera::Net::Connection::Create(1337);
		auto tcpListener = Thera::Net::tcp::Listener::Create(1337, TCPOnAccept);

		Thera::LoopWindowless(0, nullptr);
		
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	CleanupConnections();
	return 0;
}