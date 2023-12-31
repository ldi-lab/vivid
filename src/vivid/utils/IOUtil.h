#pragma once

#include <iostream>
#include <vector>
#include <map>
#include "vivid/core/Mesh.h"
#include "vivid/core/Texture.h"

namespace vivid {

class IOUtil {
public:
    IOUtil() = default;

    static bool LoadObj(const std::string& filePath,
                        std::vector<float>& positions,
                        std::vector<float> &normals);


    static bool LoadObj(const std::string& filePath,
                        std::map<std::string, MeshPtr> &meshes);

    static bool ReadObjLegacy(const std::string &filePath,
                              MeshPtr &mesh);


    static MeshPtr CreateMesh(const std::vector<unsigned int> &vertexIndices,
                              const std::vector<unsigned int > &normalIndices,
                              const std::vector<glm::vec3> &temp_vertices,
                              std::vector<glm::vec3> &temp_normals);

    static MeshPtr LoadJsonModel(const std::string &filePath);

    static TexturePtr LoadTexture(const std::string &imgPath);

    static unsigned char* LoadImage(const std::string& filePath, int& width, int& height, int& channels);

};

} // namespace