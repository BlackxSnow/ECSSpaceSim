//#include <modules/rendering/Rendering.h>
//
//#include <modules/ui/UI.h>
//
//#include <bgfx/bgfx.h>
//#include <glm/gtc/type_ptr.hpp>
//
//flecs::query<const Thera::Rendering::Renderer, const Thera::Core::WorldTransform> BuildQuery(flecs::entity camEntity, Thera::Core::MaskBehaviour mask)
//{
//	flecs::world* world = Thera::GetWorld();
//
//	flecs::query_builder query = world->query_builder<const Thera::Rendering::Renderer, const Thera::Core::WorldTransform>();
//
//	switch (mask)
//	{
//	case Thera::Core::MaskBehaviour::Whitelist:
//		query.term<Thera::Core::WhitelistedBy>(camEntity);
//		break;
//	case Thera::Core::MaskBehaviour::Blacklist:
//		query.term<Thera::Core::BlacklistedBy>(camEntity).oper(flecs::Not);
//		break;
//    default:
//        break;
//	}
//
//	return query.build();
//}
//
//void Thera::Rendering::ValidateCameraQueries(flecs::iter& iter, const Camera* cam, CachedRendererMask* mask)
//{
//	for (auto i : iter)
//	{
//		CachedRendererMask& currentMask = mask[i];
//		const Camera& currentCam = cam[i];
//
//		if (currentMask.lastMask == currentCam.masking && currentMask.maskQuery)
//		{
//			continue;
//		}
//
//		if (currentMask.maskQuery) { currentMask.maskQuery.destruct(); }
//		currentMask.maskQuery = BuildQuery(iter.entity(i), currentCam.masking);
//		currentMask.lastMask = currentCam.masking;
//	}
//}
//
//
//void RenderSingleCamera(flecs::entity entity, const Thera::Rendering::Renderer& renderer, const Thera::Core::WorldTransform& transform, const Thera::Core::WorldTransform& cam, const bgfx::ViewId target)
//{
//	if (renderer.meshes.size() == 0)
//	{
//		return;
//	}
//
//	glm::mat4 model = glm::identity<glm::mat4>();
//
//
//	glm::vec3 relativePosition = glm::vec3(transform.Position - cam.Position);
//	model = glm::scale(glm::translate(model, relativePosition) * glm::toMat4(transform.Rotation), transform.Scale);
//
//
//	bgfx::setTransform(glm::value_ptr(model));
//
//	for (auto& mesh : renderer.meshes)
//	{
//		if (mesh->Vertices.size() > 0)
//		{
//			bgfx::setVertexBuffer(0, mesh->vertexBuffer);
//		}
//		if (mesh->Indices.size() > 0)
//		{
//			bgfx::setIndexBuffer(mesh->indexBuffer);
//		}
//		bgfx::submit(target, renderer.material.shader);
//	}
//}
//
//void Thera::Rendering::RenderCameraFinal(flecs::iter& iter, const Camera* cam, const Thera::Core::WorldTransform* camTransform, const CachedRendererMask* mask)
//{
//	if (!cam->isEnabled)
//	{
//		return;
//	}
//
//	for (auto i : iter)
//	{
//		const Camera& currentCam = cam[i];
//		const Thera::Core::WorldTransform& currentCamTransform = camTransform[i];
//		const CachedRendererMask& currentMask = mask[i];
//
//		glm::mat4 view = glm::identity<glm::mat4>() * glm::toMat4(currentCamTransform.Rotation);
//		glm::mat4 proj;
//
//		if (currentCam.view == CameraView::Perspective)
//		{
//			proj = glm::perspective(currentCam.fovRad, (float)Thera::WindowWidth / (float)Thera::WindowHeight, currentCam.nearClip, currentCam.farClip);
//		}
//		else
//		{
//			// TODO: change this based on rendering API. (ZO maps clip to 0->1 for DirectX, NO maps clip to -1->1 for OpenGL. Look up the others)
//			proj = glm::orthoRH_ZO(-currentCam.size.x, currentCam.size.x, -currentCam.size.y, currentCam.size.y, currentCam.nearClip, currentCam.farClip);
//		}
//
//		// TODO: Move into camera components
//		uint64_t state = 0
//			| BGFX_STATE_WRITE_R
//			| BGFX_STATE_WRITE_G
//			| BGFX_STATE_WRITE_B
//			| BGFX_STATE_WRITE_A
//			| BGFX_STATE_WRITE_Z
//			| BGFX_STATE_DEPTH_TEST_LESS
//			| BGFX_STATE_CULL_CW
//			| BGFX_STATE_MSAA
//			;
//
//		auto target = currentCam.target;
//
//		bgfx::setViewTransform(target, glm::value_ptr(view), glm::value_ptr(proj));
//		bgfx::setViewRect(0, 0, 0, uint16_t(Thera::WindowWidth), uint16_t(Thera::WindowHeight));
//		bgfx::setState(state);
//
//		auto filter = iter.world().filter_builder<const Thera::Rendering::Renderer, const Thera::Core::WorldTransform>();
//
//		switch (currentCam.masking)
//		{
//		case Thera::Core::MaskBehaviour::Whitelist:
//			filter.term<Thera::Core::WhitelistedBy>(iter.entity(i));
//			break;
//		case Thera::Core::MaskBehaviour::Blacklist:
//			filter.term<Thera::Core::BlacklistedBy>(iter.entity(i)).oper(flecs::Not);
//			break;
//        default:
//            break;
//		}
//
//		if (currentMask.lastMask == currentCam.masking)
//		{
//			currentMask.maskQuery.each([&currentCamTransform, target](flecs::entity entity, const Renderer& renderer, const Thera::Core::WorldTransform& rendererTransform)
//			{
//				RenderSingleCamera(entity, renderer, rendererTransform, currentCamTransform, target);
//			});
//		}
//		else
//		{
//			filter.build().each([&currentCamTransform, target](flecs::entity entity, const Renderer& renderer, const Thera::Core::WorldTransform& rendererTransform)
//			{
//				RenderSingleCamera(entity, renderer, rendererTransform, currentCamTransform, target);
//			});
//		}
//	}
//}