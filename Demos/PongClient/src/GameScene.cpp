#include "GameScene.h"

void CreateScene(flecs::world& world)
{
	auto defaultShader = CCX::LoadShaderProgram("default_vs", "default_fs");
	auto paddleMesh = Thera::LoadMesh("Resources/Cube.obj");

	LeftPaddle = world.entity("LeftPaddle");
	auto leftRenderer = LeftPaddle.get_mut<Thera::Rendering::Renderer>();
	leftRenderer->meshes.push_back(paddleMesh);
	leftRenderer->material = defaultShader;
	LeftPaddle.set<Thera::Core::Transform>({ glm::dvec3(-29, 0, 0), glm::identity<glm::quat>(), glm::vec3(1,4,1) });

	RightPaddle = world.entity("RightPaddle");
	auto rightRenderer = RightPaddle.get_mut<Thera::Rendering::Renderer>();
	rightRenderer->meshes.push_back(paddleMesh);
	rightRenderer->material = defaultShader;
	RightPaddle.set<Thera::Core::Transform>({ glm::dvec3(29, 0, 0), glm::identity<glm::quat>(), glm::vec3(1,4,1) });

	Ball = world.entity("Ball");
	auto ballRenderer = Ball.get_mut<Thera::Rendering::Renderer>();
	ballRenderer->meshes.push_back(paddleMesh);
	ballRenderer->material = defaultShader;
	Ball.set<Thera::Core::Transform>({ glm::dvec3(0, 0, 0), glm::identity<glm::quat>(), glm::vec3(1,1,1) });
	Ball.add<PongBall>();

	float aspect = (float)Thera::WindowWidth / (float)Thera::WindowHeight;

	float borderPos = 22.5f;
	
	auto topBorder = world.entity("TopBorder");
	auto topRenderer = topBorder.get_mut<Thera::Rendering::Renderer>();
	topRenderer->meshes.push_back(paddleMesh);
	topRenderer->material = defaultShader;
	topBorder.set<Thera::Core::Transform>({ glm::dvec3(0, borderPos, 0), glm::identity<glm::quat>(), glm::vec3(60, 1, 1) });

	auto bottomBorder = world.entity("BottomBorder");
	auto bottomRenderer = bottomBorder.get_mut<Thera::Rendering::Renderer>();
	bottomRenderer->meshes.push_back(paddleMesh);
	bottomRenderer->material = defaultShader;
	bottomBorder.set<Thera::Core::Transform>({ glm::dvec3(0, -borderPos, 0), glm::identity<glm::quat>(), glm::vec3(60, 1, 1) });

	flecs::entity cam = world.entity();
	cam.set<Thera::Rendering::Camera>({ Thera::Rendering::CameraView::Orthographic, Thera::Core::MaskBehaviour::None, 0.1f, 100.0f, 1.0472f, glm::vec2(30, 30 / aspect) });
	cam.set<Thera::Core::Transform>({ glm::dvec3(0, 0, 10), glm::identity<glm::quat>(), glm::vec3(1,1,1) });
}