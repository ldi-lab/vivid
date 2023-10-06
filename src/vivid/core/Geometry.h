#pragma once

#include <iostream>
#include <map>
#include <memory>
#include <glad/glad.h>
#include <Eigen/Dense>
#include "Attribute.h"
#include "Shader.h"


namespace vivid {

class Geometry {
public:
    Geometry() = default;

    Geometry(const std::vector<float> *positions,
             const std::vector<float> *uvs,
             const std::vector<float> *normals);

    void AddAttribute(std::shared_ptr<Attribute> attr);

    void SetIndex(std::vector<unsigned int> &indices, bool useMove = false);

    void Translate(float x, float y, float z);

    void Rotate(float angle, const Eigen::Vector3f &axis);

    // Submit this geometry to GPU.
    unsigned int SubmitToGPU(std::shared_ptr<Shader> program);

    // Draw
    void Draw(std::shared_ptr<Shader> program, int drawMode = GL_TRIANGLES);


protected:

    static void UpdateAttribute(const std::shared_ptr<Attribute>& attr);

    // The attributes of this geometry.
    std::map<std::string, std::shared_ptr<Attribute>> attributes_;

    // Indices
    std::vector<unsigned int> indices_;

    // Vertex array object handles.
    // We create a vertex array object for each attributeLocations, so that we can
    // render the same geometry with different shaders.
    std::map<std::string, unsigned int> vaos_;

    // Index buffer object handle
    unsigned int ebo_ = 0;

};

using GeometryPtr = std::shared_ptr<Geometry>;

} //namespace vivid
