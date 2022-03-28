#pragma once

#include "glm/glm.hpp"
#include "assimp/vector3.h"

inline glm::dvec3 ToGLM(aiVector3D vec)
{
	return glm::dvec3(vec.x, vec.y, vec.z);
}