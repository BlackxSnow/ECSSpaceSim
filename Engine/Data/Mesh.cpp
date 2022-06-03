#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "../utility/ConsoleLogging.h"
#include "../utility/Conversion.h"


bgfx::VertexLayout Thera::Vertex::Layout;

/// <summary>
/// Placeholder. Loads the first mesh in a scene from a given file.
/// </summary>
/// <param name="path"></param>
/// <returns></returns>
std::shared_ptr<Thera::MeshData> Thera::LoadMesh(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcessPreset_TargetRealtime_Quality);

	if (scene == nullptr)
	{
		LogError(importer.GetErrorString(), false);
		return nullptr;
	}

	aiMesh* mesh = scene->mMeshes[0];


	std::shared_ptr<MeshData> data = std::make_shared<MeshData>();
	data->Vertices.reserve(mesh->mNumVertices);
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		data->Vertices.emplace_back(
			glm::vec3(ToGLM(mesh->mVertices[i])),
			glm::vec3(ToGLM(mesh->mNormals[i])),
			glm::vec2(ToGLM(mesh->mTextureCoords[0][i])));
	}

	size_t indexCount = mesh->mNumFaces * static_cast<size_t>(3);
	if (indexCount > std::numeric_limits<uint16_t>::max())
	{
		LogWarning("Unable to load mesh - index count exceeds uint16 limit!");
		return nullptr;
	}

	data->Indices.reserve(mesh->mNumFaces * static_cast<size_t>(3));

	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace& face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)
		{
			data->Indices.push_back(face.mIndices[j]);
		}
	}

	data->vertexBuffer = bgfx::createVertexBuffer(bgfx::makeRef(data->Vertices.data(), sizeof(Thera::Vertex) * data->Vertices.size()), Thera::Vertex::Layout);
	data->indexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(data->Indices.data(), sizeof(uint16_t) * data->Indices.size()));
	return data;
}