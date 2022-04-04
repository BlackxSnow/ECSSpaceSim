#pragma once

#include <vector>
#include <memory>
#include <flecs.h>
#include <bgfx/bgfx.h>
#include <glm/mat4x4.hpp>

#include "../../Data/Mesh.h"
#include "../Core/Core.h"
#include "../../Engine.h"

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

			Material(bgfx::ProgramHandle shader)
			{
				this->shader = shader;
			}
			Material() {}
		};

		struct Renderer
		{
			std::vector<std::shared_ptr<MeshData>> meshes;
			Material material;
		};

		struct Mask
		{
			ecse::Core::MaskBehaviour lastMask = (ecse::Core::MaskBehaviour)-1;
			flecs::query<const ecse::Rendering::Renderer, const ecse::Core::WorldTransform*> maskQuery;
		};

		enum class CameraView
		{
			Perspective,
			Orthographic
		};

		struct Camera
		{
			CameraView view;
			ecse::Core::MaskBehaviour masking;
			float nearClip;
			float farClip;
			float fovRad;
			glm::vec2 size;
			bool isEnabled = true;
			bgfx::ViewId target;
		};

		static void RenderCameraFinal(flecs::iter& iter, const Camera* cam, const ecse::Core::WorldTransform* camTransform, const Mask* mask);
		
		inline static flecs::query<const Camera, Mask> maskValidationQuery;
		static void ValidateCameraQueries(flecs::iter& iter, const Camera* cam, Mask* mask);

		Rendering(flecs::world& world)
		{
			world.module<Rendering>();

			world.component<Renderer>();

			world.component<Camera>()
				.member<int>("View Type")
				.member<int>("Mask Type")
				.member<float, flecs::units::length::Meters>("Clip Distance", 2)
				.member<float, flecs::units::angle::Radians>("fovRad")
				.member<float>("Ortho size", 2)
				.add(flecs::With, world.id<ecse::Core::Transform>())
				.add(flecs::With, world.id<Mask>());

			world.system<const Camera, const ecse::Core::WorldTransform, const Mask>("RenderCameraFinal")
				.iter(RenderCameraFinal);

			maskValidationQuery = world.query_builder<const Camera, Mask>().build();
			OnFinalValidate.Register("Rendering::ValidateCameraQueries", []() {maskValidationQuery.iter(ValidateCameraQueries); });
		}
	};
	
	


}