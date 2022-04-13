#include "Controls.h"

void sim::Controls::TransformControllables(flecs::iter& iter, ecse::Core::Transform* transform, const Controllable* controllable, const ecse::Core::WorldTransform* worldTransform)
{
	glm::vec2 delta = ecse::Input::GetAction("Rotation")->GetData<glm::dvec2>();
	
	glm::quat hRotation = glm::angleAxis(delta.x * controllable->sensitivity, worldTransform->Up());
	
	transform->Rotation = transform->Rotation * hRotation;

	glm::vec3 vAxis = -worldTransform->Right();
	glm::quat vRotation = glm::angleAxis(-delta.y * controllable->sensitivity, vAxis);
	
	transform->Rotation = transform->Rotation * vRotation;

	glm::vec2 input = glm::vec2(0);

	input.x -= ecse::Input::GetAction("Left")->GetData<float>();
	input.x += ecse::Input::GetAction("Right")->GetData<float>();
	input.y += ecse::Input::GetAction("Forward")->GetData<float>();
	input.y -= ecse::Input::GetAction("Back")->GetData<float>();
	
	glm::vec3 forward = worldTransform->Forward();

	LogInfo(std::to_string(forward.x) + ", " + std::to_string(forward.y) + ", " + std::to_string(forward.z));

	if (input.x != 0 || input.y != 0)
	{
		input = glm::normalize(input);
		transform->Position += input.y * worldTransform->Forward() * controllable->speed * iter.delta_time();
		transform->Position += input.x * worldTransform->Right() * controllable->speed * iter.delta_time();
	}
}
