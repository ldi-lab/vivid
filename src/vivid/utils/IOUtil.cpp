#include "IOUtil.h"
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#define STB_IMAGE_IMPLEMENTATION  //necessary for stb_image.h
#include "vivid/utils/stb_image.h"

namespace vivid {

bool IOUtil::LoadObj(const std::string &filePath,
                     std::vector<float> &positions,
                     std::vector<float> &normals) {
    // open file
    std::ifstream ifs(filePath);
    if (!ifs.is_open()) {
        std::cerr << "failed to open file: " << filePath << std::endl;
        return false;
    }

    positions.clear();
    normals.clear();

    std::vector<unsigned int> vertexIndices, normalIndices;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec3> temp_normals;

    std::string aline, s, s1;
    float x, y, z;
    while (!ifs.eof()) {
        getline(ifs, aline);
        if (aline.empty()) {
            continue;
        }
        std::stringstream ss(aline);
        ss >> s;
        if (s == "v") { // vertex position
            ss >> x >> y >> z;
            temp_vertices.emplace_back(x, y, z);
        } else if (s == "vn") {
            ss >> x >> y >> z;
            temp_normals.emplace_back(x, y, z);
        }
        else if (s == "f") {  // face
            for (int i = 0; i < 3; i++) {
                ss >> s;
                std::stringstream stream(s);
                getline(stream, s1, '/');
                vertexIndices.push_back( stoi(s1) - 1);
                getline(stream, s1, '/');
                getline(stream, s1, '/');
                normalIndices.push_back( stoi(s1) - 1);
            }
        }
    }

    for (size_t i = 0; i < vertexIndices.size(); ++i) {
        const auto& position = temp_vertices[vertexIndices[i]];
        const auto& normal = temp_normals[normalIndices[i]];
        positions.push_back(position.x);
        positions.push_back(position.y);
        positions.push_back(position.z);
        normals.push_back(normal.x);
        normals.push_back(normal.y);
        normals.push_back(normal.z);
    }

    std::cout << "Done reading obj: " << (positions.size() / 3) << " vertices\n";
    return true;
}


bool IOUtil::ReadObjLegacy(const std::string &filePath, vivid::MeshPtr &mesh) {
    std::vector<float> positions, normals;
    LoadObj(filePath, positions, normals);
    auto geometry = std::make_shared<Geometry>(&positions, nullptr, &normals);
    mesh = std::make_shared<Mesh>(geometry);
    return true;
}



bool IOUtil::LoadObj(const std::string &filePath, std::map<std::string, MeshPtr> &meshes) {
    meshes.clear();

    // open file
    std::ifstream ifs(filePath);
    if (!ifs.is_open()) {
        std::cerr << "failed to open file: " << filePath << std::endl;
        return false;
    }

    std::vector<unsigned int> vertexIndices, uvIndices, normalIndices;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;

    std::string aline, s, s1, lastName;
    float x, y, z;
    while (!ifs.eof()) {
        getline(ifs, aline);
        if (aline.empty()) {
            continue;
        }
        std::stringstream ss(aline);
        ss >> s;

        if (s == "o") {
            ss >> s;
            if (!lastName.empty() && !temp_vertices.empty()) {
                auto mesh = CreateMesh(vertexIndices, uvIndices, normalIndices, temp_vertices, temp_uvs, temp_normals);
                meshes[lastName] = mesh;
            }
            lastName = s;
            temp_vertices.clear();
            temp_uvs.clear();
            temp_normals.clear();
            vertexIndices.clear();
            uvIndices.clear();
            normalIndices.clear();
        }

        if (s == "v") { // vertex position
            ss >> x >> y >> z;
            temp_vertices.emplace_back(x, y, z);
        } else if (s == "vn") {
            ss >> x >> y >> z;
            temp_normals.emplace_back(x, y, z);
        }  else if (s == "vt") {
            ss >> x >> y;
            temp_uvs.emplace_back(x, y);
        }
        else if (s == "f") {  // face
            for (int i = 0; i < 3; i++) {
                ss >> s;
                std::stringstream stream(s);
                getline(stream, s1, '/');
                vertexIndices.push_back( stoi(s1) - 1);
                getline(stream, s1, '/');
                uvIndices.push_back( stoi(s1) - 1);
                getline(stream, s1, '/');
                normalIndices.push_back( stoi(s1) - 1);
            }
        }
    }

    if (!lastName.empty() && !temp_vertices.empty()) {
        auto mesh = CreateMesh(vertexIndices, uvIndices, normalIndices, temp_vertices, temp_uvs, temp_normals);
        meshes[lastName] = mesh;
    }

    return true;
}


MeshPtr IOUtil::CreateMesh(const std::vector<unsigned int> &vertexIndices,
                           const std::vector<unsigned int> &uvIndices,
                           const std::vector<unsigned int> &normalIndices,
                           const std::vector<glm::vec3> &temp_vertices,
                           const std::vector<glm::vec2> &temp_uvs,
                           std::vector<glm::vec3> &temp_normals) {
    std::vector<float> positions, normals, uvs;
    for (size_t i = 0; i < vertexIndices.size(); ++i) {
        const auto& position = temp_vertices[vertexIndices[i]];
        const auto& normal = temp_normals[normalIndices[i]];
        const auto& uv = temp_uvs[uvIndices[i]];
        positions.push_back(position.x);
        positions.push_back(position.y);
        positions.push_back(position.z);
        normals.push_back(normal.x);
        normals.push_back(normal.y);
        normals.push_back(normal.z);
        uvs.push_back(uv.x);
        uvs.push_back(uv.y);
    }

    auto geometry = std::make_shared<Geometry>(&positions, &uvs, &normals);
    auto mesh = std::make_shared<Mesh>(geometry);
    return mesh;
}


unsigned char* IOUtil::LoadImage(const std::string &filePath, int &width, int &height, int &channels) {
    return stbi_load(filePath.c_str(), &width, &height, &channels, 0);
}


} // vivid