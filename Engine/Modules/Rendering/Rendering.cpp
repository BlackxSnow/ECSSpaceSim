#include "Rendering.h"

#include "../UI/UI.h"

#include <bgfx/bgfx.h>
#include <glm/gtc/type_ptr.hpp>

flecs::query<const Thera::Rendering::Renderer, const Thera::Core::WorldTransform*> BuildQuery(flecs::entity camEntity, Thera::Core::MaskBehaviour mask)
{
	flecs::world* world = Thera::GetWorld();

	flecs::query_builder query = world->query_builder<const Thera::Rendering::Renderer, const Thera::Core::WorldTransform*>();

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

void Thera::Rendering::ValidateCameraQueries(flecs::iter& iter, const Camera* cam, CachedRendererMask* mask)
{
	if (mask->lastMask == cam->masking && mask->maskQuery)
	{
		iter.skip();
		return;
	}
	if (mask->maskQuery) { mask->maskQuery.destruct(); }
	mask->maskQuery = BuildQuery(iter.entity(0), cam->masking);
	mask->lastMask = cam->masking;
}


void RenderSingleCamera(flecs::iter& iter, const Thera::Rendering::Renderer* renderer, const Thera::Core::WorldTransform* transform, const Thera::Core::WorldTransform* cam, const bgfx::ViewId target)
{
	if (renderer->meshes.size() == 0)
	{
		return;
	}

	glm::mat4 model = glm::identity<glm::mat4>();
	
	if (transform != nullptr)
	{
		glm::vec3 relativePosition = glm::vec3(transform->Position - cam->Position);
		model = glm::scale(glm::translate(model, relativePosition) * glm::toMat4(transform->Rotation), transform->Scale);
	}
	else
	{
		model = glm::translate(model, glm::vec3(-cam->Position));
	}

	bgfx::setTransform(glm::value_ptr(model));

	for (auto& mesh : renderer->meshes)
	{
		if (mesh->Vertices.size() > 0)
		{
			bgfx::setVertexBuffer(0, mesh->vertexBuffer);
		}
		if (mesh->Indices.size() > 0)
		{
			bgfx::setIndexBuffer(mesh->indexBuffer);
		}
		bgfx::submit(target, renderer->material.shader);
	}
}

void Thera::Rendering::RenderCameraFinal(flecs::iter& iter, const Camera* cam, const Thera::Core::WorldTransform* camTransform, const CachedRendererMask* mask)
{
	if (!cam->isEnabled)
	{
		return;
	}

	glm::mat4 view = glm::identity<glm::mat4>() * glm::toMat4(camTransform->Rotation);
	glm::mat4 proj;

	if (cam->view == CameraView::Perspective)
	{
		proj = glm::perspective(cam->fovRad, (float)Thera::WindowWidth / (float)Thera::WindowHeight, cam->nearClip, cam->farClip);
	}
	else
	{
		proj = glm::ortho(-cam->size.x, cam->size.x, -cam->size.y, cam->size.y, cam->nearClip, cam->farClip);
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

	auto target = cam->target;

	bgfx::setViewTransform(target, glm::value_ptr(view), glm::value_ptr(proj));
	bgfx::setViewRect(0, 0, 0, uint16_t(Thera::WindowWidth), uint16_t(Thera::WindowHeight));
	bgfx::setState(state);

	auto filter = iter.world().filter_builder<const Thera::Rendering::Renderer, const Thera::Core::WorldTransform*>();

	switch (cam->masking)
	{
	case Thera::Core::MaskBehaviour::Whitelist:
		filter.term<Thera::Core::WhitelistedBy>(iter.entity(0));
		break;
	case Thera::Core::MaskBehaviour::Blacklist:
		filter.term<Thera::Core::BlacklistedBy>(iter.entity(0)).oper(flecs::Not);
		break;
	}

	if (mask->lastMask == cam->masking)
	{
		mask->maskQuery.iter([camTransform, target](flecs::iter& iter, const Renderer* renderer, const Thera::Core::WorldTransform* rendererTransform)
		{
			RenderSingleCamera(iter, renderer, rendererTransform, camTransform, target);
		});
	}
	else
	{
		filter.build().iter([camTransform, target](flecs::iter& iter, const Renderer* renderer, const Thera::Core::WorldTransform* rendererTransform)
		{
			RenderSingleCamera(iter, renderer, rendererTransform, camTransform, target);
		});
	}
}