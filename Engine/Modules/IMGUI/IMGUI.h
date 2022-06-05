#pragma once

#include <flecs.h>


namespace Thera::IMGUI
{
	struct Module
	{
		Module(flecs::world& world);
		~Module();
	};
}