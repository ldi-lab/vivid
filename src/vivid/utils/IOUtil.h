#pragma once

#include <iostream>
#include <vector>
#include <map>
#include "vivid/core/Mesh.h"

namespace vivid {

class IOUtil {
public:
    IOUtil() = default;

    static bool LoadObj(const std::string& filePath,
                        std::vector<float>& positions,
                        std::vector<float> &normals,
                        std::vector<float> &uvs);


    static bool LoadObj(const std::string& filePath,
                        std::map<std::string, MeshPtr> &meshes);


    static MeshPtr CreateMesh(const std::vector<unsigned int> &vertexIndices,
                              const std::vector<unsigned int> &uvIndices,
                              const std::vector<unsigned int > &normalIndices,
                              const std::vector<glm::vec3> &temp_vertices,
                              const std::vector<glm::vec2> &temp_uvs,
                              std::vector<glm::vec3> &temp_normals);

    static unsigned char* LoadImage(const std::string& filePath, int& width, int& height, int& channels);

};

} // namespace