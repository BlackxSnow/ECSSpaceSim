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
#include "Modules/Input/Input.h"
#include "Modules/UI/UI.h"
#include "Modules/IMGUI/IMGUI.h"

int Thera::WindowWidth = 1024;
int Thera::WindowHeight = 768;

CCX::Event<> Thera::OnInit;
CCX::Event<> Thera::OnInputPoll;
CCX::Event<> Thera::OnFinalValidate;

std::vector<GLFWwindow*> Windows;
flecs::world* World;
flecs::entity GameRoot;

size_t currentFrame = 1;

const bgfx::ViewId kClearView = 0;

static void glfw_error(int error, const char* description)
{
	LogError(description, false);
}

const std::vector<GLFWwindow*>& Thera::GetWindows()
{
	return Windows;
}
flecs::world* Thera::GetWorld()
{
	return World;
}
flecs::entity* Thera::GetGameRoot()
{
	return &GameRoot;
}

size_t Thera::FrameCount()
{
	return currentFrame;
}

static GLFWwindow* InitialiseGLFW(std::function<void()>* setHints = nullptr)
{
	glfwSetErrorCallback(glfw_error);
	if (!glfwInit())
	{
		LogError("Unable to initialise GLFW", true);
	}
	if (setHints != nullptr)
	{
		(*setHints)();
	}
	GLFWwindow* window = glfwCreateWindow(Thera::WindowWidth, Thera::WindowHeight, "SpaceSim", nullptr, nullptr);
	if (!window)
	{
		LogError("Unable to create GLFW Window", true);
	}
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

	init.resolution.width = (uint32_t)Thera::WindowWidth;
	init.resolution.height = (uint32_t)Thera::WindowHeight;
	init.resolution.reset = BGFX_RESET_VSYNC;
	if (!bgfx::init(init))
	{
		LogError("Unable to intiialise BGFX", true);
	}
}

static void HandleWindowResize(GLFWwindow* window, const bgfx::ViewId& clearView)
{
	int oldWidth = Thera::WindowWidth, oldHeight = Thera::WindowHeight;
	glfwGetWindowSize(window, &Thera::WindowWidth, &Thera::WindowHeight);

	if (oldWidth != Thera::WindowWidth || oldHeight != Thera::WindowHeight)
	{
		bgfx::reset((uint32_t)Thera::WindowWidth, (uint32_t)Thera::WindowHeight, BGFX_RESET_VSYNC);
		bgfx::setViewRect(clearView, 0, 0, bgfx::BackbufferRatio::Equal);
		Thera::WindowResized.Invoke(Thera::WindowWidth, Thera::WindowHeight);
	}
}

void Thera::Init()
{
	Windows.push_back(InitialiseGLFW());
	InitialiseBGFX(Windows[0]);
	Vertex::Init();
	
	bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH);

	World = new flecs::world();
	GameRoot = World->entity("Game");

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

	World->set_pipeline(pipeline);

	World->import<flecs::units>();
	World->import<Thera::Core>();
	World->import<Thera::Rendering>();
	World->import<Thera::UI::Module>();
	World->import<Thera::IMGUI::Module>();

	Input::Initialise();
	OnInit.Invoke();

	World->set<flecs::rest::Rest>({});
}

void Thera::TestInit()
{
	std::function<void()> hints = []() { glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); };
	Windows.push_back(InitialiseGLFW(&hints));
	Input::Initialise();
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

int Thera::Loop(int argc, char** argv)
{
	auto lastTime = std::chrono::system_clock::now();
	double delta = 0;
	while (!glfwWindowShouldClose(Windows[0])) 
	{
		glfwPollEvents();
		OnInputPoll.Invoke();
		HandleWindowResize(Windows[0], kClearView);

		bgfx::touch(kClearView);

		delta = GetDelta(lastTime);
		World->progress(delta);

		OnFinalValidate.Invoke();

		bgfx::frame();
		currentFrame++;
	}

	Shutdown();
	return 0;
}