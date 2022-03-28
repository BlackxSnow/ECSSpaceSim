#pragma once

#include <glm/glm.hpp>
#include <Vector>

/// <summary>
/// Stores data for a mesh vertex.
/// </summary>
struct Vertex
{
	glm::dvec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoord;

	Vertex(glm::dvec3 pos, glm::dvec3 normal, glm::dvec2 uv)
	{
		Position = pos;
		Normal = (glm::vec3)normal;
		TexCoord = (glm::vec2)uv;
	}
	Vertex()
	{

	}
};

struct MeshData
{
	std::vector<Vertex> Vertices;
	std::vector<int> Indices;
};