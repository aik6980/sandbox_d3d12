#include "model_loader.h"
#include "math/aabb.h"
#include "mesh_data.h"

void Model_loader::load(const string& file)
{
	std::filesystem::path asset_file_path = std::filesystem::current_path().parent_path();
	asset_file_path.concat("/assets/" + file);

	Assimp::Importer importer;

	const aiScene* p_scene = importer.ReadFile(asset_file_path.string(), aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);
	if (!p_scene) {
		DBG::OutputString("Model_loader:load() File not found, %s", file.c_str());
		return;
	}

	process_node(p_scene->mRootNode, p_scene);
}

void Model_loader::process_node(aiNode* node, const aiScene* scene)
{
	for (UINT i = 0; i < node->mNumMeshes; ++i) {
		uint32_t mesh_id = node->mMeshes[i];
		aiMesh*	 ai_mesh = scene->mMeshes[mesh_id];

		auto&& mesh = process_mesh(ai_mesh, scene);
		m_meshes.emplace_back(mesh);
	}

	for (UINT i = 0; i < node->mNumChildren; i++) {
		this->process_node(node->mChildren[i], scene);
	}
}

Mesh_data Model_loader::process_mesh(aiMesh* mesh, const aiScene* scene)
{
	Mesh_vertex_array vertices;
	vertices.reset_vertices(mesh->mNumVertices);

	Aabb3 aabb;
	// Walk through each of the mesh's vertices
	for (UINT i = 0; i < mesh->mNumVertices; i++) {

		auto&& pos	  = vertices.m_position[i];
		auto&& colour = vertices.m_colour[i];
		auto&& normal = vertices.m_normal[i];

		pos = XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		// expand aabb
		aabb.expand(pos);

		if (mesh->mColors[0]) {
			auto&& ai_colour = mesh->mColors[0][i]; // rgba float

			colour = Color(ai_colour.r, ai_colour.g, ai_colour.b);
		}

		if (mesh->mNormals) {
			auto&& ai_normal = mesh->mNormals[i]; // rgba float

			normal = XMFLOAT3(ai_normal.x, ai_normal.y, ai_normal.z);
		}

		// if (mesh->mTextureCoords[0]) {
		//  vertex.texcoord.x = (float)mesh->mTextureCoords[0][i].x;
		//  vertex.texcoord.y = (float)mesh->mTextureCoords[0][i].y;
		//}
	}

	Mesh_index_array indices;
	indices.reset_indices(mesh->mNumFaces * 3);
	UINT index_count = 0;
	for (UINT i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++) {
			if (face.mNumIndices != 3) {
				throw;
			}

			indices.m_indices32[index_count] = face.mIndices[j];
			index_count++;
		}
	}

	// if (mesh->mMaterialIndex >= 0) {
	//	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	//
	//	std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
	//	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// }

	Mesh_data mesh_data;
	mesh_data.m_vertices = vertices;
	mesh_data.m_indices	 = indices;
	mesh_data.m_aabb	 = aabb;

	return mesh_data;
}
