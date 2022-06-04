#include "UI.h"

#include "../../utility/ConsoleLogging.h"

namespace Thera::UI
{
	flecs::query<const Thera::Core::WorldTransform, const Thera::UI::ScreenRenderer> BuildUIQuery(flecs::entity camEntity, Thera::Core::MaskBehaviour mask)
	{
		flecs::world* world = Thera::GetWorld();

		flecs::query_builder query = world->query_builder< const Thera::Core::WorldTransform, const Thera::UI::ScreenRenderer>();

		switch (mask)
		{
		case Thera::Core::MaskBehaviour::Whitelist:
			query.term<Thera::Core::WhitelistedBy>(camEntity);
			break;
		case Thera::Core::MaskBehaviour::Blacklist:
			query.term<Thera::Core::BlacklistedBy>(camEntity).oper(flecs::Not);
			break;
		}

		return query.build();
	}

	void Module::_ValidateUIMasks(flecs::iter& iter, const Thera::Rendering::Camera* camera, CachedUIMask* maskCache)
	{
		if (maskCache->lastMask == camera->masking && maskCache->maskQuery)
		{
			iter.skip();
			return;
		}

		if (maskCache->maskQuery) { maskCache->maskQuery.destruct(); }
		maskCache->maskQuery = BuildUIQuery(iter.entity(0), camera->masking);
		maskCache->lastMask = camera->masking;
	}

	void Module::RenderSingleScreenRenderer(flecs::iter& iter, const Thera::Core::WorldTransform* rendererTransform, const Thera::UI::ScreenRenderer* renderer)
	{
		bgfx::setTexture(0, _UITextureUniform, renderer->Texture);
		bgfx::submit(0, _ScreenShader);
	}

	void Module::_RenderCameraUI(flecs::iter& iter, const Thera::Rendering::Camera* camera, const Thera::Core::WorldTransform* camTransform, const CachedUIMask* maskCache)
	{
		if (!camera->isEnabled)
		{
			return;
		}

		// TODO: Move into camera components
		uint64_t state = 0
			| BGFX_STATE_WRITE_R
			| BGFX_STATE_WRITE_G
			| BGFX_STATE_WRITE_B
			| BGFX_STATE_WRITE_A
			| BGFX_STATE_WRITE_Z
			| BGFX_STATE_DEPTH_TEST_LESS
			| BGFX_STATE_CULL_CW
			| BGFX_STATE_MSAA
			;

		bgfx::setViewRect(0, 0, 0, uint16_t(Thera::WindowWidth), uint16_t(Thera::WindowHeight));
		bgfx::setState(state);

		flecs::filter_builder filter = iter.world().filter_builder<const Thera::Core::WorldTransform, const Thera::UI::ScreenRenderer>();

		switch (camera->masking)
		{
		case Thera::Core::MaskBehaviour::Whitelist:
			filter.term<Thera::Core::WhitelistedBy>(iter.entity(0));
			break;
		case Thera::Core::MaskBehaviour::Blacklist:
			filter.term<Thera::Core::BlacklistedBy>(iter.entity(0)).oper(flecs::Not);
			break;
		}

		bgfx::setVertexBuffer(0, _ScreenVertexBuffer);
		bgfx::setIndexBuffer(_ScreenIndexBuffer);

		if (maskCache->lastMask == camera->masking)
		{
			maskCache->maskQuery.iter([camTransform](flecs::iter& iter, const Thera::Core::WorldTransform* rendererTransform, const Thera::UI::ScreenRenderer* renderer)
			{
				RenderSingleScreenRenderer(iter, rendererTransform, renderer);
			});
		}
		else
		{
			filter.build().iter([camTransform](flecs::iter& iter, const Thera::Core::WorldTransform* rendererTransform, const Thera::UI::ScreenRenderer* renderer)
			{
				RenderSingleScreenRenderer(iter, rendererTransform, renderer);
			});
		}
	}
}