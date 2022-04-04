#include "Engine.h"

#define flecs_STATIC

#include "bgfx/bgfx.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "utility/ConsoleLogging.h"
#include "utility/CCXShader.h"

#include <filesystem>
#include <Modules/Rendering/Rendering.h>

int main()
{
	ecse::Init();

	auto defaultShader = CCX::LoadShaderProgram("default_vs", "default_fs");

	auto cubeMesh = ecse::LoadMesh("Resources/Cube.obj");

	flecs::entity cam = ecse::GetWorld()->entity();
	cam.set<ecse::Rendering::Camera>({ ecse::Rendering::CameraView::Perspective, ecse::Core::MaskBehaviour::None, 0.1f, 100.0f, 1.0472f });
	cam.set<ecse::Core::Transform>({ glm::dvec3(0, 0, 10), glm::identity<glm::quat>(), glm::vec3(1,1,1) });

	flecs::entity cube = ecse::GetWorld()->entity();
	auto cubeRenderer = cube.get_mut<ecse::Rendering::Renderer>();
	cubeRenderer->meshes.push_back(cubeMesh);
	cubeRenderer->material.shader = defaultShader;

	cube.set<ecse::Core::Transform>({ glm::dvec3(3, 3, -10), glm::identity<glm::quat>(), glm::vec3(1, 1, 1) });

	

	bgfx::setDebug(BGFX_DEBUG_NONE);

	ecse::Loop(0, nullptr);
}