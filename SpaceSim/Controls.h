#pragma once

#include "Modules/Input/Input.h"
#include "Modules/Core/Core.h"

namespace sim
{
	inline void BuildControls()
	{
		namespace inp = ecse::Input;

		inp::SetBindContext(inp::CreateAction("Left", inp::Output::Scalar));
		inp::CreateBinding(inp::Key::A);

		inp::SetBindContext(inp::CreateAction("Right", inp::Output::Scalar));
		inp::CreateBinding(inp::Key::D);

		inp::SetBindContext(inp::CreateAction("Forward", inp::Output::Scalar));
		inp::CreateBinding(inp::Key::W);

		inp::SetBindContext(inp::CreateAction("Back", inp::Output::Scalar));
		inp::CreateBinding(inp::Key::S);

		inp::SetBindContext(inp::CreateAction("Rotation", inp::Output::Vector2));
		inp::CreateBinding(inp::Mouse::Delta);
	}

	struct Controls
	{

		struct Controllable
		{
			float speed = 1;
			float sensitivity = 0.1f;
		};

		static void TransformControllables(flecs::iter& iter, ecse::Core::Transform* transform, const Controllable* controllable, const ecse::Core::WorldTransform* worldTransform);

		Controls(flecs::world& world)
		{
			world.module<Controls>();

			world.component<Controllable>()
				.member<float>("Speed")
				.member<float>("Sensitivity")
				.add(flecs::With, world.id<ecse::Core::Transform>());

			world.system<ecse::Core::Transform, const Controllable, const ecse::Core::WorldTransform>().iter(TransformControllables);
		}
	};
}