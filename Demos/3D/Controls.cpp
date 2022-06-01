#include "Controls.h"

void sim::Controls::TransformControllables(flecs::iter& iter, Thera::Core::Transform* transform, const Controllable* controllable, const Thera::Core::WorldTransform* worldTransform)
{
	glm::vec2 delta = Thera::Input::GetAction("Rotation")->GetData<glm::dvec2>();
	
	glm::quat hRotation = glm::angleAxis(delta.x * controllable->sensitivity, worldTransform->Up());
	
	transform->Rotation = transform->Rotation * hRotation;

	glm::vec3 vAxis = -worldTransform->Right();
	glm::quat vRotation = glm::angleAxis(-delta.y * controllable->sensitivity, vAxis);
	
	transform->Rotation = transform->Rotation * vRotation;

	glm::vec2 input = Thera::Input::GetAction("Move")->GetData<glm::vec2>();
	
	glm::vec3 forward = worldTransform->Forward();

	if (input.x != 0 || input.y != 0)
	{
		input = glm::normalize(input);
		transform->Position += input.y * worldTransform->Forward() * controllable->speed * iter.delta_time();
		transform->Position += input.x * worldTransform->Right() * controllable->speed * iter.delta_time();
	}
}
