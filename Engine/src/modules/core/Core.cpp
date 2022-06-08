#include <modules/core/Core.h>

void Thera::Core::UpdateWorldTransforms(flecs::iter& it, const Transform* local, WorldTransform* world)
{
	if (!it.changed())
	{
		it.skip();
		return;
	}

	for (auto i : it)
	{
		const Transform& currentLocal = local[i];
		WorldTransform& currentWorld = world[i];
		if (it.is_set(3))
		{
			const WorldTransform& parent = it.term<const WorldTransform>(3)[i];


			currentWorld.Position = currentLocal.Position + parent.Position;
			currentWorld.Rotation = currentLocal.Rotation * parent.Rotation;
			currentWorld.Scale = currentLocal.Scale * parent.Scale;
		}
		else
		{
			currentWorld.Position = currentLocal.Position;
			currentWorld.Rotation = currentLocal.Rotation;
			currentWorld.Scale = currentLocal.Scale;
		}
	}
}

glm::vec3 Thera::Core::WorldTransform::Forward() const
{
	glm::vec3 forward = glm::vec3(0, 0, -1);
	forward = forward * Rotation;
	return glm::normalize(forward);
}

glm::vec3 Thera::Core::WorldTransform::Right() const
{
	glm::vec3 right = glm::vec3(1, 0, 0);
	right = right * Rotation;
	return glm::normalize(right);
}

glm::vec3 Thera::Core::WorldTransform::Up() const
{
	glm::vec3 up = glm::vec3(0,1,0);
	up = up * Rotation;
	return glm::normalize(up);
}
