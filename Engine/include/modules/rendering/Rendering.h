#pragma once

#include <vector>
#include <memory>
#include <flecs.h>
#include <bgfx/bgfx.h>
#include <glm/mat4x4.hpp>

#include <modules/rendering/Mesh.h>
#include <modules/core/Core.h>
#include <Engine.h>

namespace Thera
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

		struct CachedRendererMask
		{
			Thera::Core::MaskBehaviour lastMask = (Thera::Core::MaskBehaviour)-1;
			flecs::query<const Thera::Rendering::Renderer, const Thera::Core::WorldTransform> maskQuery;
		};

		enum class CameraView
		{
			Perspective,
			Orthographic
		};

		struct Camera
		{
			CameraView view;
			Thera::Core::MaskBehaviour masking;
			float nearClip;
			float farClip;
			float fovRad;
			glm::vec2 size;
			bool isEnabled = true;
			bgfx::ViewId target = 0;
		};

		static void RenderCameraFinal(flecs::iter& iter, const Camera* cam, const Thera::Core::WorldTransform* camTransform, const CachedRendererMask* mask);
		
		inline static flecs::query<const Camera, CachedRendererMask> _MaskValidationQuery;
		static void ValidateCameraQueries(flecs::iter& iter, const Camera* cam, CachedRendererMask* mask);

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
				.add(flecs::With, world.id<Thera::Core::Transform>())
				.add(flecs::With, world.id<CachedRendererMask>());

			world.system<const Camera, const Thera::Core::WorldTransform, const CachedRendererMask>("RenderCameraFinal")
				.iter(RenderCameraFinal);

			_MaskValidationQuery = world.query_builder<const Camera, CachedRendererMask>().build();
			OnFinalValidate.Register([]() {_MaskValidationQuery.iter(ValidateCameraQueries); });
		}
	};
	
	


}