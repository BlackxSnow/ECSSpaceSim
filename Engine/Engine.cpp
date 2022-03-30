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
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <chrono>

#include "utility/ConsoleLogging.h"
#include "Data/Mesh.h"
#include "utility/Conversion.h"

int ecse::WindowWidth = 1024;
int ecse::WindowHeight = 768;

CCX::Event<double> ecse::OnUpdate;
CCX::Event<> ecse::OnDraw;

GLFWwindow* Window;
const bgfx::ViewId kClearView = 0;

static void glfw_error(int error, const char* description)
{
	LogError(description, false);
}

static void glfw_key(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
	//TODO replace this function with an input manager
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

/// <summary>
/// Placeholder. Loads the first mesh in a scene from a given file.
/// </summary>
/// <param name="path"></param>
/// <returns></returns>
std::shared_ptr<MeshData> ecse::LoadMesh(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcessPreset_TargetRealtime_Quality);

	if (scene == nullptr)
	{
		LogError(importer.GetErrorString(), false);
		return nullptr;
	}
	
	aiMesh* mesh = scene->mMeshes[0];
	

	std::shared_ptr<MeshData> data = std::make_shared<MeshData>();
	data->Vertices.reserve(mesh->mNumVertices);
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		data->Vertices.emplace_back(
			glm::vec3(ToGLM(mesh->mVertices[i])),
			glm::vec3(ToGLM(mesh->mNormals[i])),
			glm::vec2(ToGLM(mesh->mTextureCoords[0][i])));
	}

	size_t indexCount = mesh->mNumFaces * static_cast<size_t>(3);
	if (indexCount > MAXUINT16)
	{
		LogWarning("Unable to load mesh - index count exceeds uint16 limit!")
	}

	data->Indices.reserve(mesh->mNumFaces * static_cast<size_t>(3));

	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace& face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)
		{
			data->Indices.push_back(face.mIndices[j]);
		}
	}
	return data;
}

void ecse::Init()
{
	Window = InitialiseGLFW();
	InitialiseBGFX(Window);
	Vertex::Init();
	
	bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH);
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
	while (!glfwWindowShouldClose(Window)) 
	{
		glfwPollEvents();
		HandleWindowResize(Window, kClearView);

		delta = GetDelta(lastTime);
		OnUpdate.Invoke(delta);

		bgfx::touch(kClearView);

		OnDraw.Invoke();

		bgfx::frame();
	}
	return 0;
}