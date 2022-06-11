#pragma once

#include <memory>
#include <string>
#include <vector>
#include <flecs.h>
#include <GLFW/glfw3.h>

#include <modules/rendering/Mesh.h>
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

	/// <summary>
	/// Return all instantiated GLFW windows.
	/// </summary>
	/// <returns></returns>
	const std::vector<GLFWwindow*>& GetWindows();
	std::vector<GLFWwindow*>& GetWindowsMutable();
	/// <summary>
	/// Return the Flecs world. This is generally unsafe to mutate during Loop().
	/// </summary>
	/// <returns></returns>
	flecs::world* GetWorld();
	flecs::entity* GetGameRoot();

	/// <summary>
	/// Return the number of frames since application start.
	/// </summary>
	/// <returns></returns>
	size_t FrameCount();

	/// <summary>
	/// Invoked after internal initialisation.
	/// </summary>
	extern CCX::Event<> OnInit;
	/// <summary>
	/// Invoked each time GLFW input is polled (pre-pipeline).
	/// </summary>
	extern CCX::Event<> OnInputPoll;
	/// <summary>
	/// Invoked after pipeline has concluded each frame.
	/// </summary>
	extern CCX::Event<> OnFinalValidate;

	/// <summary>
	/// Defer a function call until the Flecs world is next writable.
	/// </summary>
	/// <param name="operation"></param>
	void Defer(std::function<void()> operation);

	double DeltaTime();

	void Init();
	void InitWindowless();
	void TestInit();
	int Loop(int argc, char** argv);
	int LoopWindowless(int argc, char** argv);
}