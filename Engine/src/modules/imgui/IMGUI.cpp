#include <modules/imgui/IMGUI.h>

#include <Engine.h>
#include <utility/ConsoleLogging.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include "internal/imgui_impl_bgfx.h"

namespace Thera::IMGUI
{
	Module::Module(flecs::world& world)
	{
		world.module<Module>();

		world.system("IMGUI Initialiser").kind(flecs::PreFrame).iter([](flecs::iter& iter) {
			ImGui_Implbgfx_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		});

		world.system("IMGUI Render").kind(flecs::PostRender).iter([](flecs::iter& iter) {
			ImGui::Render();
			ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());
		});

		Thera::OnInit.Register([]() {
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

			bool glfw = ImGui_ImplGlfw_InitForOther(Thera::GetWindows()[0], true);
			if (!glfw)
			{
				LogError("Failed to initialise IMGUI for GLFW.", true);
			}
			ImGui_Implbgfx_Init(1);
		});
	}
	Module::~Module()
	{
		ImGui_Implbgfx_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
}