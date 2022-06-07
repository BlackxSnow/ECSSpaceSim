#pragma once
#include <Engine.h>
#include <imgui.h>

namespace imgui = ImGui;

inline std::string PlayerName = "";

void BuildUI(flecs::iter& iter);