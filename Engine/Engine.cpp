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
#include <memory>

#include "utility/ConsoleLogging.h"
#include "Data/Mesh.h"
#include "utility/Conversion.h"
#include "Engine.h"

int WindowWidth = 1024;
int WindowHeight = 768;

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
	GLFWwindow* window = glfwCreateWindow(WindowWidth, WindowHeight, "SpaceSim", nullptr, nullptr);
	if (!window)
	{
		LogError("Unable to create GLFW Window", true);
	}
	glfwSetKeyCallback(window, glfw_key);
	return window;
}

static void InitialiseBGFX(GLFWwindow* window)
{
	bgfx::renderFrame();
	bgfx::Init init;

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
	init.platformData.ndt = glfwGetX11Display();
	init.platformData.nwh = (void*)(uintptr_t)glfwGetX11Window(window);
#elif BX_PLATFORM_OSX
	init.platformData.nwh = glfwGetCocoaWindow(window);
#elif BX_PLATFORM_WINDOWS
	init.platformData.nwh = glfwGetWin32Window(window);
#endif

	init.resolution.width = (uint32_t)WindowWidth;
	init.resolution.height = (uint32_t)WindowHeight;
	init.resolution.reset = BGFX_RESET_VSYNC;
	if (!bgfx::init(init))
	{
		LogError("Unable to intiialise BGFX", true);
	}
}

static void HandleWindowResize(GLFWwindow* window, const bgfx::ViewId& clearView)
{
	int oldWidth = WindowWidth, oldHeight = WindowHeight;
	glfwGetWindowSize(window, &WindowWidth, &WindowHeight);

	if (oldWidth != WindowWidth || oldHeight != WindowHeight)
	{
		bgfx::reset((uint32_t)WindowWidth, (uint32_t)WindowHeight, BGFX_RESET_VSYNC);
		bgfx::setViewRect(clearView, 0, 0, bgfx::BackbufferRatio::Equal);
	}
}

/// <summary>
/// Placeholder. Loads the first mesh in a scene from a given file.
/// </summary>
/// <param name="path"></param>
/// <returns></returns>
static std::shared_ptr<MeshData> LoadMesh(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

	if (scene == nullptr)
	{
		LogError(importer.GetErrorString(), false);
		return nullptr;
	}
	
	aiMesh* mesh = scene->mMeshes[0];
	

	std::shared_ptr<MeshData> data = std::make_shared<MeshData>();
	data->Vertices.resize(mesh->mNumVertices);
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		//Vertex vert = Vertex(ToGLM(mesh->mVertices[i]),
		//	ToGLM(mesh->mNormals[i]),
		//	glm::dvec2(ToGLM(mesh->mTextureCoords[0][i])));

		data->Vertices.emplace_back(
			ToGLM(mesh->mVertices[i]),
			ToGLM(mesh->mNormals[i]),
			glm::dvec2(ToGLM(mesh->mTextureCoords[0][i])));
	}

	data->Indices.resize(mesh->mNumFaces * static_cast<size_t>(3));

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

int Loop(int argc, char** argv)
{
	GLFWwindow* window = InitialiseGLFW();
	InitialiseBGFX(window);
	
	const bgfx::ViewId kClearView = 0;
	bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR);
	bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);

	// Test
	auto cubeMesh = LoadMesh("Resources/Cube.obj");

	while (!glfwWindowShouldClose(window)) 
	{
		glfwPollEvents();
		HandleWindowResize(window, kClearView);

		bgfx::touch(kClearView);

		bgfx::frame();
	}
	return 0;
}