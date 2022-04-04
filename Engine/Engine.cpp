#include "Engine.h"

#include <bx/bx.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <GLFW/glfw3.h>

#if BX_PLATFORM_LINUX
#define GLFW_EXPOSE_NATIVE_X11
#elif BX_PLATFORM_WINDOWS
#define GLFW_EXPOSE_NATIVE_WIN32
#elif BX_PLATFORM_OSX
#define GLFW_EXPOSE_NATIVE_COCOA
#endif

#include <GLFW/glfw3native.h>
#include <chrono>

#include "utility/ConsoleLogging.h"
#include "Data/Mesh.h"
#include "utility/Conversion.h"
#include "Modules/Core/Core.h"
#include "Modules/Rendering/Rendering.h"

int ecse::WindowWidth = 1024;
int ecse::WindowHeight = 768;

CCX::Event<> ecse::OnFinalValidate;

std::vector<GLFWwindow*> Windows;
flecs::world* World;

const bgfx::ViewId kClearView = 0;

static void glfw_error(int error, const char* description)
{
	LogError(description, false);
}

static void glfw_key(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
	//TODO replace this function with an input manager
}

const std::vector<GLFWwindow*>& ecse::GetWindows()
{
	return Windows;
}
flecs::world* ecse::GetWorld()
{
	return World;
}

static GLFWwindow* InitialiseGLFW()
{
	glfwSetErrorCallback(glfw_error);
	if (!glfwInit())
	{
		LogError("Unable to initialise GLFW", true);
	}
	GLFWwindow* window = glfwCreateWindow(ecse::WindowWidth, ecse::WindowHeight, "SpaceSim", nullptr, nullptr);
	if (!window)
	{
		LogError("Unable to create GLFW Window", true);
	}
	glfwSetKeyCallback(window, glfw_key);
	return window;
}

static void InitialiseBGFX(GLFWwindow* window)
{
	bgfx::Init init;

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
	init.platformData.ndt = glfwGetX11Display();
	init.platformData.nwh = (void*)(uintptr_t)glfwGetX11Window(window);
#elif BX_PLATFORM_OSX
	init.platformData.nwh = glfwGetCocoaWindow(window);
#elif BX_PLATFORM_WINDOWS
	init.platformData.nwh = glfwGetWin32Window(window);
#endif

	init.resolution.width = (uint32_t)ecse::WindowWidth;
	init.resolution.height = (uint32_t)ecse::WindowHeight;
	init.resolution.reset = BGFX_RESET_VSYNC;
	if (!bgfx::init(init))
	{
		LogError("Unable to intiialise BGFX", true);
	}
}

static void HandleWindowResize(GLFWwindow* window, const bgfx::ViewId& clearView)
{
	int oldWidth = ecse::WindowWidth, oldHeight = ecse::WindowHeight;
	glfwGetWindowSize(window, &ecse::WindowWidth, &ecse::WindowHeight);

	if (oldWidth != ecse::WindowWidth || oldHeight != ecse::WindowHeight)
	{
		bgfx::reset((uint32_t)ecse::WindowWidth, (uint32_t)ecse::WindowHeight, BGFX_RESET_VSYNC);
		bgfx::setViewRect(clearView, 0, 0, bgfx::BackbufferRatio::Equal);
	}
}

void ecse::Init()
{
	Windows.push_back(InitialiseGLFW());
	InitialiseBGFX(Windows[0]);
	Vertex::Init();
	
	bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH);

	World = new flecs::world();

	World->import<flecs::units>();
	World->import<ecse::Core>();
	World->import<ecse::Rendering>();

	flecs::PreRender = World->entity();
	flecs::OnRender = World->entity();
	flecs::PostRender = World->entity();


	auto pipeline = World->pipeline("ECSEPipeline");
	pipeline.add(flecs::PreFrame);
	pipeline.add(flecs::OnUpdate);
	pipeline.add(flecs::OnValidate);
	pipeline.add(flecs::OnRender);
	pipeline.add(flecs::PostRender);
	pipeline.add(flecs::PostFrame);

	World->set<flecs::rest::Rest>({});
}

void Shutdown()
{
	delete World;
}

double GetDelta(std::chrono::system_clock::time_point& last)
{
	auto now = std::chrono::system_clock::now();
	double delta = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1,1>>>(now - last).count();
	last = now;
	return delta;
}

int ecse::Loop(int argc, char** argv)
{
	auto lastTime = std::chrono::system_clock::now();
	double delta = 0;
	while (!glfwWindowShouldClose(Windows[0])) 
	{
		glfwPollEvents();
		HandleWindowResize(Windows[0], kClearView);

		bgfx::touch(kClearView);

		delta = GetDelta(lastTime);
		World->progress(delta);

		OnFinalValidate.Invoke();

		bgfx::frame();
	}

	Shutdown();
	return 0;
}