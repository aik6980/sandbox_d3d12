#pragma once

#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>

#include "mesh_data.h"

class Model_loader {
  public:
    void load(const string& file);

    const Mesh_data& meshes() { return m_meshes[0]; }

  private:
    void      process_node(aiNode* node, const aiScene* scene);
    Mesh_data process_mesh(aiMesh* mesh, const aiScene* scene);

    vector<Mesh_data> m_meshes;
};
