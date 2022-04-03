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
