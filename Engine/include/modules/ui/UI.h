#pragma once
#include <flecs.h>
#include <bgfx/bgfx.h>
#include <array>

#include "../Core/Core.h"
#include "../Rendering/Rendering.h"
#include "../../utility/CCXShader.h"

namespace Thera::UI
{
	/// <summary>
	/// Renders a texture in screen space.
	/// </summary>
	struct ScreenRenderer
	{
		bgfx::TextureHandle Texture;
	};

	/// <summary>
	/// cached Query of ScreenRenderers.
	/// </summary>
	struct CachedUIMask
	{
		Thera::Core::MaskBehaviour lastMask = (Thera::Core::MaskBehaviour)-1;
		flecs::query<const Thera::Core::WorldTransform, const Thera::UI::ScreenRenderer> maskQuery;
	};

	/// <summary>
	/// UI module definition for flecs.
	/// </summary>
	struct Module
	{
	private:
		/// <summary>
		/// The query to iterate when validating CachedUIMask components.
		/// </summary>
		inline static flecs::query<const Thera::Rendering::Camera, CachedUIMask> _MaskValidationQuery;
		static void _ValidateUIMasks(flecs::iter& iter, const Thera::Rendering::Camera* camera, CachedUIMask* maskCache);

		/// <summary>
		/// Renders the UI for a single camera using the cached query in its CachedUIMask.
		/// </summary>
		/// <param name="iter"></param>
		/// <param name="camera"></param>
		/// <param name="camTransform"></param>
		/// <param name="maskCache"></param>
		static void _RenderCameraUI(flecs::iter& iter, const Thera::Rendering::Camera* camera, const Thera::Core::WorldTransform* camTransform, const CachedUIMask* maskCache);

		static void RenderSingleScreenRenderer(flecs::iter& iter, const Thera::Core::WorldTransform* rendererTransform, const Thera::UI::ScreenRenderer* renderer);

		inline static std::array<glm::vec3, 4> _ScreenVertices { glm::vec3(-1,-1,0), glm::vec3(1,-1,0), glm::vec3(1,1,0), glm::vec3(-1,1,0) };
		inline static std::array<uint16_t, 6> _ScreenIndices { 0, 1, 2, 2, 3, 0 };
		inline static bgfx::VertexLayout _PositionVertex;
		inline static bgfx::VertexBufferHandle _ScreenVertexBuffer;
		inline static bgfx::IndexBufferHandle _ScreenIndexBuffer;
		inline static bgfx::UniformHandle _UITextureUniform;
		inline static bgfx::ProgramHandle _ScreenShader;
	public:

		Module(flecs::world& world)
		{
			world.module<Module>();

			world.component<ScreenRenderer>().add(flecs::With, world.id<Thera::Core::WorldTransform>());
			world.component<CachedUIMask>();

			// Ensure all cameras are added with a CachedUIMask. (Thanks Flecs!)
			world.component<Thera::Rendering::Rendering::Camera>().add(flecs::With, world.id<CachedUIMask>());

			world.system< const Thera::Rendering::Camera, const Thera::Core::WorldTransform, const CachedUIMask>("RenderCameraUI").iter(_RenderCameraUI);

			// Validate queries post-frame (World is ephemeral during system runs and can't be used to make queries)
			_MaskValidationQuery = world.query_builder<const Thera::Rendering::Camera, CachedUIMask>().build();
			Thera::OnFinalValidate.Register([]() { _MaskValidationQuery.iter(_ValidateUIMasks); });

			// bgfx resource iniitalisation
			_PositionVertex.begin().add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float).end();
			_ScreenVertexBuffer = bgfx::createVertexBuffer(bgfx::makeRef(&_ScreenVertices, sizeof(glm::vec3) * 4), _PositionVertex);
			_ScreenIndexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(&_ScreenIndices, sizeof(uint16_t) * 6));
			_UITextureUniform = bgfx::createUniform("texColour", bgfx::UniformType::Sampler);
			_ScreenShader = CCX::LoadShaderProgram("screen_vs", "screen_fs");
		}
	};
}