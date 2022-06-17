#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <glm/common.hpp>

#include "ConsoleLogging.h"
#include "CCXType.h"

namespace CCX
{
	template<class T>
	T Max(const T& a, const T& b) { static_assert(false_type<T>(), "Unsupported type"); }
	template<>
	inline float Max<float>(const float& a, const float& b)
	{
		return (a > b) ? a : b;
	}
	template<>
	inline double Max<double>(const double& a, const double& b)
	{
		return (a > b) ? a : b;
	}
	template<>
	inline glm::vec2 Max<glm::vec2>(const glm::vec2& a, const glm::vec2& b)
	{
		return glm::max(a, b);
	}
	template<>
	inline glm::vec3 Max<glm::vec3>(const glm::vec3& a, const glm::vec3& b)
	{
		return glm::max(a, b);
	}
	template<>
	inline glm::dvec2 Max<glm::dvec2>(const glm::dvec2& a, const glm::dvec2& b)
	{
		return glm::max(a, b);
	}
	template<>
	inline glm::dvec3 Max<glm::dvec3>(const glm::dvec3& a, const glm::dvec3& b)
	{
		return glm::max(a, b);
	}
}