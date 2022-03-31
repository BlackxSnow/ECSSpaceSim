#include "Engine.h"

#define flecs_STATIC

#include "Components/Camera.h"
#include "bgfx/bgfx.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "utility/ConsoleLogging.h"
#include "utility/CCXShader.h"

#include <filesystem>

int main()
{
	ecse::Init();

	auto cubeMesh = ecse::LoadMesh("Resources/Cube.obj");
	PerspectiveCamera cam = PerspectiveCamera{ 60, 0.1, 100 };

	auto vbh = bgfx::createVertexBuffer(bgfx::makeRef(cubeMesh->Vertices.data(), sizeof(ecse::Vertex) * cubeMesh->Vertices.size()), ecse::Vertex::Layout);
	auto ibh = bgfx::createIndexBuffer(bgfx::makeRef(cubeMesh->Indices.data(), sizeof(uint16_t) * cubeMesh->Indices.size()));


	auto defaultShader = CCX::LoadShaderProgram("default_vs", "default_fs");

	uint64_t state = 0
		| BGFX_STATE_WRITE_R 
		| BGFX_STATE_WRITE_G 
		| BGFX_STATE_WRITE_B 
		| BGFX_STATE_WRITE_A 
		| BGFX_STATE_WRITE_Z
		| BGFX_STATE_DEPTH_TEST_LESS
		| BGFX_STATE_CULL_CW
		| BGFX_STATE_MSAA
		;

	bgfx::setDebug(BGFX_DEBUG_NONE);
	glm::mat4 model = glm::identity<glm::mat4>();
	model = glm::translate(model, glm::vec3(-5, 5, 0));
	
	ecse::OnDraw.Register("draw1", [model, cam, state, ibh, vbh, defaultShader]() 
	{
		glm::mat4 proj = glm::perspective(1.0472f, (float)ecse::WindowWidth / (float)ecse::WindowHeight, cam.nearClip, cam.farClip);
		glm::mat4 view = glm::lookAt(glm::vec3(0, 0, -35), glm::vec3(0, 0, 0), glm::vec3(0,1,0));
		bgfx::setViewTransform(0, glm::value_ptr(view), glm::value_ptr(proj));
		bgfx::setViewRect(0, 0, 0, uint16_t(ecse::WindowWidth), uint16_t(ecse::WindowHeight));

		bgfx::setTransform(glm::value_ptr(model));
		bgfx::setVertexBuffer(0, vbh);
		bgfx::setIndexBuffer(ibh);
		bgfx::setState(state);
		bgfx::submit(0, defaultShader); 
	});
	ecse::Loop(0, nullptr);
}