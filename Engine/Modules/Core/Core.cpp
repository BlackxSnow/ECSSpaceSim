#include "Core.h"

void ecse::Core::UpdateWorldTransforms(flecs::iter& it, const Transform* local, WorldTransform* world)
{
	if (!it.changed())
	{
		it.skip();
		return;
	}

	if (it.is_set(3))
	{
		const WorldTransform& parent = it.term<const WorldTransform>(3)[0];

		world->Position = local->Position + parent.Position;
		world->Rotation = local->Rotation * parent.Rotation;
		world->Scale = local->Scale * parent.Scale;
	}
	else
	{
		world->Position = local->Position;
		world->Rotation = local->Rotation;
		world->Scale = local->Scale;
	}
}

glm::vec3 ecse::Core::WorldTransform::Forward() const
{
	glm::vec3 forward = glm::vec3(0, 0, -1);
	forward = Rotation * forward;
	return glm::normalize(forward);
}

glm::vec3 ecse::Core::WorldTransform::Right() const
{
	glm::vec3 right = glm::vec3(1, 0, 0);
	right = Rotation * right;
	return glm::normalize(right);
}

glm::vec3 ecse::Core::WorldTransform::Up() const
{
	glm::vec3 up = glm::vec3(0,1,0);
	up = Rotation * up;
	return glm::normalize(up);
}
