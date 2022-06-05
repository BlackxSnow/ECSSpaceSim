#pragma once

#include <memory>
#include <string>
#include <vector>
#include <flecs.h>
#include <GLFW/glfw3.h>

#include "Data/Mesh.h"
#include "utility/CCXEvent.h"

namespace flecs
{
	// Pipeline phases
	inline flecs::entity_t PreRender;
	inline flecs::entity_t OnRender;
	inline flecs::entity_t PostRender;
}

namespace Thera
{
	extern int WindowWidth;
	extern int WindowHeight;
	
	// This will need to be changed when multiple windows are implemented.
	inline CCX::Event<int, int> WindowResized;

	const std::vector<GLFWwindow*>& GetWindows();
	flecs::world* GetWorld();
	flecs::entity* GetGameRoot();

	size_t FrameCount();

	extern CCX::Event<> OnInit;
	extern CCX::Event<> OnInputPoll;
	extern CCX::Event<> OnFinalValidate;

	void Init();
	void TestInit();
	int Loop(int argc, char** argv);
}