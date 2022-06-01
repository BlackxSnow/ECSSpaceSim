#pragma once

#include "Modules/Input/Input.h"
#include "Modules/Core/Core.h"

namespace sim
{
	inline void BuildControls()
	{
		namespace inp = Thera::Input;

		inp::SetBindContext(inp::CreateAction("Move", inp::Output::Vector2, Thera::Input::Precision::Single));
		Thera::Input::CreateCompositeBinding(Thera::Input::Output::Vector2, Thera::Input::Precision::Single,
		{
			Thera::Input::CreateConstituent(Thera::Input::Key::A, { Thera::Input::Component::NegX }),
			Thera::Input::CreateConstituent(Thera::Input::Key::D, { Thera::Input::Component::PosX }),
			Thera::Input::CreateConstituent(Thera::Input::Key::S, { Thera::Input::Component::NegY }),
			Thera::Input::CreateConstituent(Thera::Input::Key::W, { Thera::Input::Component::PosY })
		});

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

		static void TransformControllables(flecs::iter& iter, Thera::Core::Transform* transform, const Controllable* controllable, const Thera::Core::WorldTransform* worldTransform);

		Controls(flecs::world& world)
		{
			world.module<Controls>();

			world.component<Controllable>()
				.member<float>("Speed")
				.member<float>("Sensitivity")
				.add(flecs::With, world.id<Thera::Core::Transform>());

			world.system<Thera::Core::Transform, const Controllable, const Thera::Core::WorldTransform>().iter(TransformControllables);
		}
	};
}