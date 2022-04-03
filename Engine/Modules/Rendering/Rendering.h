#pragma once

#include <vector>
#include <memory>
#include <flecs.h>
#include <bgfx/bgfx.h>
#include <glm/mat4x4.hpp>

#include "../../Data/Mesh.h"
#include "../Core/Core.h"

namespace ecse
{
	/// <summary>
	/// FLECS rendering module for ECSE
	/// </summary>
	struct Rendering
	{
		struct Material
		{
			bgfx::ProgramHandle shader;

		};

		struct Renderer
		{
			std::vector<std::shared_ptr<MeshData>> meshes;
		};

		struct Perspective
		{
			ecse::Core::MaskBehaviour masking;
			float nearClip;
			float farClip;
			float fovRad;
		};

		struct Orthographic
		{
			ecse::Core::MaskBehaviour masking;
			float nearClip;
			float farClip;
			glm::vec2 size;
		};

		Rendering(flecs::world& world)
		{
			world.module<Rendering>();

			world.component<Renderer>();

			world.component<Perspective>()
				.member<int>("masking")
				.member<float, flecs::units::length::Meters>("nearClip")
				.member<float, flecs::units::length::Meters>("farClip")
				.member<float, flecs::units::angle::Radians>("fovRad")
				.add(flecs::With, world.id<ecse::Core::Transform>());
			world.component<Orthographic>()
				.member<int>("masking")
				.member<float, flecs::units::length::Meters>("nearClip")
				.member<float, flecs::units::length::Meters>("farClip")
				.add(flecs::With, world.id<ecse::Core::Transform>());
		}
	};
	
	


}