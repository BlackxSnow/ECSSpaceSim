#pragma once

#include <glm/mat4x4.hpp>

// TODO figure out how to integrate this into ECS. Can we inherit from Camera for orthographic and perspective cameras?
enum class CameraView
{
	Perspective,
	Orthographic
};

struct Camera
{
	CameraView ViewType;

};

struct PerspectiveCamera
{
	float FOV;
	float nearClip;
	float farClip;
};