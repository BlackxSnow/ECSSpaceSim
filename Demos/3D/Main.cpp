#include "Engine.h"

#define flecs_STATIC

#include "bgfx/bgfx.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "utility/ConsoleLogging.h"
#include "utility/CCXShader.h"
#include "Controls.h"

#include <filesystem>
#include <Modules/Rendering/Rendering.h>

int main()
{

	ecse::OnInit.Register([]() { ecse::GetWorld()->import<sim::Controls>(); });
	ecse::Init();
	sim::BuildControls();

	auto prev = ecse::GetWorld()->set_scope(*ecse::GetGameRoot());

	auto defaultShader = CCX::LoadShaderProgram("default_vs", "default_fs");

	auto cubeMesh = ecse::LoadMesh("Resources/Cube.obj");

	flecs::entity cam = ecse::GetWorld()->entity();
	cam.set<ecse::Rendering::Camera>({ ecse::Rendering::CameraView::Perspective, ecse::Core::MaskBehaviour::None, 0.1f, 100.0f, 1.0472f });
	cam.set<ecse::Core::Transform>({ glm::dvec3(0, 0, 10), glm::identity<glm::quat>(), glm::vec3(1,1,1) });
	cam.set<sim::Controls::Controllable>({ 10.0f, 0.01f });

	flecs::entity parent1 = ecse::GetWorld()->entity();
	parent1.set<ecse::Core::Transform>({ glm::dvec3(0,0,1) });
	ecse::GetWorld()->set_scope(parent1);
	flecs::entity parent2 = ecse::GetWorld()->entity();
	parent2.set<ecse::Core::Transform>({ glm::dvec3(0,-2,0) });
	ecse::GetWorld()->set_scope(parent2);

	flecs::entity cube = ecse::GetWorld()->entity();
	auto cubeRenderer = cube.get_mut<ecse::Rendering::Renderer>();
	cubeRenderer->meshes.push_back(cubeMesh);
	cubeRenderer->material.shader = defaultShader;

	cube.set<ecse::Core::Transform>({ glm::dvec3(3, 3, -10), glm::identity<glm::quat>(), glm::vec3(1, 1, 1) });

	

	bgfx::setDebug(BGFX_DEBUG_NONE);

	ecse::GetWorld()->set_scope(prev);

	ecse::Loop(0, nullptr);
}