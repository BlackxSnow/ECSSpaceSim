#pragma once

#include <glm/glm.hpp>
#include <Vector>
#include <memory>
#include <string>
#include "bgfx/bgfx.h"



namespace ecse
{
	/// <summary>
	/// Stores data for a mesh vertex.
	/// </summary>
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoord;

		Vertex(glm::vec3 pos, glm::vec3 normal, glm::vec2 uv)
		{
			Position = pos;
			Normal = normal;
			TexCoord = uv;
		}
		Vertex()
		{
			Position = glm::vec3(0);
			Normal = glm::vec3(0);
			TexCoord = glm::vec2(0);
		}

		static void Init()
		{
			Layout.begin()
				.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
				.add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
				.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
				.end();
		}

		static bgfx::VertexLayout Layout;
	};

	struct MeshData
	{
		std::vector<Vertex> Vertices;
		std::vector<uint16_t> Indices;
	};

	std::shared_ptr<MeshData> LoadMesh(const std::string& path);
}