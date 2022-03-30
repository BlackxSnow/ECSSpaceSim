#pragma once

#include <memory>
#include <string>

#include "Data/Mesh.h"
#include "utility/CCXEvent.h"

namespace ecse
{
	extern int WindowWidth;
	extern int WindowHeight;

	extern CCX::Event<double> OnUpdate;
	extern CCX::Event<> OnDraw;

	std::shared_ptr<MeshData> LoadMesh(const std::string& path);
	void Init();
	int Loop(int argc, char** argv);
}