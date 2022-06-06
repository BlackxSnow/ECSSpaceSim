#pragma once

#include <flecs.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Thera
{
	struct Core
	{
		/// <summary>
		/// Defines masking behaviour for some components (eg. camera).
		/// Whitelist and blacklist are defined by flecs relationships.
		/// eg. a renderer that "blacklists" a camera will not be rendered by it.
		/// </summary>
		enum class MaskBehaviour
		{
			None,
			Whitelist,
			Blacklist
		};

		/// <summary>
		/// Relation tag to indicate an entity is assigned to another's behaviour (eg. camera).
		/// </summary>
		struct WhitelistedBy {};
		/// <summary>
		/// Relation tag to indicate an entity is not assigned to another's behaviour (eg. camera).
		/// </summary>
		struct BlacklistedBy {};

		/// <summary>
		/// Component that describes spatial information for an entity.
		/// </summary>
		struct Transform
		{
			glm::dvec3 Position;
			glm::quat Rotation;
			glm::vec3 Scale;

			Transform(glm::dvec3 p = glm::dvec3(0,0,0), glm::quat r = glm::identity<glm::quat>(), glm::vec3 s = glm::vec3(1,1,1))
			{
				Position = p;
				Rotation = r;
				Scale = s;
			}
		};

		struct WorldTransform
		{
			glm::dvec3 Position;
			glm::quat Rotation;
			glm::vec3 Scale;

			glm::vec3 Forward() const;
			glm::vec3 Right() const;
			glm::vec3 Up() const;

			WorldTransform()
			{
				Position = glm::dvec3(0, 0, 0);
				Rotation = glm::identity<glm::quat>();
				Scale = glm::vec3(1, 1, 1);
			}
			
			WorldTransform& operator=(WorldTransform&&) = default;
			WorldTransform(WorldTransform&&) = default;
			WorldTransform(WorldTransform&) = default;
			~WorldTransform() = default;
		};

		struct Model
		{
			glm::mat4 Model;
		};

		static void UpdateWorldTransforms(flecs::iter& it, const Transform* local, WorldTransform* world);

		Core(flecs::world& world)
		{
			world.module<Core>();

			world.component<WorldTransform>()
				.member<double, flecs::units::length::Meters>("Position", 3)
				.member<float>("Rotation", 4)
				.member<float>("Scale", 3);

			world.component<Transform>()
				.member<double, flecs::units::length::Meters>("Position", 3)
				.member<float>("Rotation", 4)
				.member<float>("Scale", 3)
				.add(flecs::With, world.id<WorldTransform>());


			world.component<WhitelistedBy>();
			world.component<BlacklistedBy>();

			world.system<const Transform, WorldTransform>("UpdateWorldTransforms").kind(flecs::OnValidate)
				.term<const WorldTransform>().super(flecs::ChildOf, flecs::Cascade).oper(flecs::Optional)
				.iter(UpdateWorldTransforms);
		}
	};


}