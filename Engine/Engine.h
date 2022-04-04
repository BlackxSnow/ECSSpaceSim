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

namespace ecse
{
	extern int WindowWidth;
	extern int WindowHeight;
	
	const std::vector<GLFWwindow*>& GetWindows();
	flecs::world* GetWorld();

	extern CCX::Event<> OnFinalValidate;

	void Init();
	int Loop(int argc, char** argv);
}