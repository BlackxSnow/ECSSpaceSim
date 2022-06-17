#include "Engine.h"

#include "bgfx/bgfx.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "utility/ConsoleLogging.h"
#include "utility/CCXShader.h"
#include "Controls.h"

#include <filesystem>
#include <modules/rendering/Rendering.h>
#include <modules/ui/UI.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int main()
{

	Thera::OnInit.Register([]() { Thera::GetWorld()->import<sim::Controls>(); });
	Thera::Init();
	sim::BuildControls();

	//auto img = imageLoad("./Resources/test.png", bgfx::TextureFormat::RGBA32F);
	int width, height, channels;
	auto img = stbi_load("./Resources/test.png", &width, &height, &channels, 0);

	flecs::entity texTest = Thera::GetWorld()->entity();
	texTest.set<Thera::UI::ScreenRenderer>({ bgfx::createTexture2D(width, height, false, 1, bgfx::TextureFormat::RGB8, 0, bgfx::makeRef(img, width * height * channels))});

	auto prev = Thera::GetWorld()->set_scope(*Thera::GetGameRoot());

	auto defaultShader = CCX::LoadShaderProgram("default_vs", "default_fs");

	auto cubeMesh = Thera::LoadMesh("Resources/Cube.obj");

	flecs::entity cam = Thera::GetWorld()->entity();
	cam.set<Thera::Rendering::Camera>({ Thera::Rendering::CameraView::Perspective, Thera::Core::MaskBehaviour::None, 0.1f, 100.0f, 1.0472f });
	cam.set<Thera::Core::Transform>({ glm::dvec3(0, 0, 10), glm::identity<glm::quat>(), glm::vec3(1,1,1) });
	cam.set<sim::Controls::Controllable>({ 10.0f, 0.01f });

	flecs::entity parent1 = Thera::GetWorld()->entity();
	parent1.set<Thera::Core::Transform>({ glm::dvec3(0,0,1) });
	Thera::GetWorld()->set_scope(parent1);
	flecs::entity parent2 = Thera::GetWorld()->entity();
	parent2.set<Thera::Core::Transform>({ glm::dvec3(0,-2,0) });
	Thera::GetWorld()->set_scope(parent2);

	flecs::entity cube = Thera::GetWorld()->entity();
	auto cubeRenderer = cube.get_mut<Thera::Rendering::Renderer>();
	cubeRenderer->meshes.push_back(cubeMesh);
	cubeRenderer->material.shader = defaultShader;

	cube.set<Thera::Core::Transform>({ glm::dvec3(3, 3, -10), glm::identity<glm::quat>(), glm::vec3(1, 1, 1) });

	

	bgfx::setDebug(BGFX_DEBUG_NONE);

	Thera::GetWorld()->set_scope(prev);

	Thera::Loop(0, nullptr);
}