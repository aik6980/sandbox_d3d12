#pragma once

#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>

class Model_loader {
  private:
    void load(const string& file);
};
