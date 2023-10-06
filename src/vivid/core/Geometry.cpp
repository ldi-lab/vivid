//
// Created by d on 23-5-4.
//

#include "Geometry.h"
#include <glad/glad.h>
#include <utility>

namespace vivid {

Geometry::Geometry(const std::vector<float> *positions,
                   const std::vector<float> *uvs,
                   const std::vector<float> *normals) {
    if (positions != nullptr) {
        auto positionAttr = std::make_shared<Attribute>(AttributeType::Position, 3, false, positions);
        AddAttribute(positionAttr);
    }
    if (uvs != nullptr) {
        auto uvAttr = std::make_shared<Attribute>(AttributeType::TexCoord0, 2, false, uvs);
        AddAttribute(uvAttr);
    }
    if (normals != nullptr) {
        auto normalAttr = std::make_shared<Attribute>(AttributeType::Normal, 3, false, normals);
        AddAttribute(normalAttr);
    }
}


void Geometry::AddAttribute(std::shared_ptr<Attribute> attr) {
    attributes_[attr->Name()] = std::move(attr);
}


void Geometry::SetIndex(std::vector<unsigned int> &indices, bool useMove) {
    indices_ = useMove ? std::move(indices) : indices;
}


void Geometry::Translate(float x, float y, float z) {
    if (attributes_.find("position") == attributes_.end()) {
        return;
    }
    std::vector<float> positions = attributes_["position"]->GetData();
    for (int i = 0; i < positions.size(); i += 3) {
        positions[i] += x;
        positions[i + 1] += y;
        positions[i + 2] += z;
    }
    attributes_["position"]->SetData(positions);
}


void Geometry::Rotate(float angle, const Eigen::Vector3f &axis) {
    Eigen::AngleAxisf rotation(angle, axis);
    if (attributes_.find("position") != attributes_.end()) {
        std::vector<float> positions = attributes_["position"]->GetData();
        for (int i = 0; i < positions.size(); i += 3) {
            Eigen::Vector3f v(positions[i], positions[i + 1], positions[i + 2]);
            v = rotation * v;
            positions[i] = v.x();
            positions[i + 1] = v.y();
            positions[i + 2] = v.z();
        }
        attributes_["position"]->SetData(positions);
    }

    if (attributes_.find("normal") != attributes_.end()) {
        std::vector<float> normals = attributes_["normal"]->GetData();
        for (int i = 0; i < normals.size(); i += 3) {
            Eigen::Vector3f v(normals[i], normals[i + 1], normals[i + 2]);
            v = rotation * v;
            normals[i] = v.x();
            normals[i + 1] = v.y();
            normals[i + 2] = v.z();
        }
        attributes_["normal"]->SetData(normals);
    }
}


unsigned int Geometry::SubmitToGPU(std::shared_ptr<Shader> program) {
    unsigned int vao;
    // Create vertex array
    glGenVertexArrays(1, &vao);

    // Bind the vertex array
    glBindVertexArray(vao);

    // Create GL buffers and submit data
    for (const auto &it : attributes_) {
        UpdateAttribute(it.second);
    }

    // Submit index buffer
    if (!indices_.empty()) {
        glGenBuffers(1, &ebo_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizei)(indices_.size() * sizeof(unsigned int)), &indices_[0], GL_STATIC_DRAW);
    }

    // Set vertex attribute pointer
    const std::map<int, std::string>& attributeLocations = program->AttributeLocations();
    for (const auto &it : attributeLocations) {
        const int loc = it.first;
        const std::string& name = it.second;
        if (attributes_.find(name) != attributes_.end()) {
            const auto& attr = attributes_[name];
            glBindBuffer(GL_ARRAY_BUFFER, attr->VBO());
            glEnableVertexAttribArray(loc);
            glVertexAttribPointer(loc,                          // attribute location
                                  attr->ElementsPerItem(),      // size
                                  GL_FLOAT,                        // type
                                  attr->Normalized(),              // normalized ?
                                  (GLsizei)attr->ItemSize(),     // stride
                                  (GLvoid*)0                          // array buffer offset
                                  );
        }
    }

    // Unbind the vertex array
    glBindVertexArray(0);

    return vao;
}


void Geometry::UpdateAttribute(const std::shared_ptr<Attribute>& attr) {
    const bool isNewBuffer = !attr->VBO();

    // Create GL buffer
    if (isNewBuffer) {
        unsigned int vbo = 0;
        glGenBuffers(1, &vbo);
        attr->SetVBO(vbo);
    }

    // Bind buffer and submit data
    glBindBuffer(GL_ARRAY_BUFFER, attr->VBO());
    if (isNewBuffer) {
        glBufferData(GL_ARRAY_BUFFER, (GLsizei)(attr->TotalSize()), attr->GetData().data(), GL_STATIC_DRAW);
    } else {
        glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizei)(attr->TotalSize()), attr->GetData().data());
    }

}


void Geometry::Draw(std::shared_ptr<Shader> program, int drawMode) {
    const std::string &attributeLocationsStr = program->AttributeLocationsStr();
    unsigned int vao = 0;
    if (vaos_.find(attributeLocationsStr) == vaos_.end()) {
        vao = SubmitToGPU(program);
        vaos_[attributeLocationsStr] = vao;
    } else {
        vao = vaos_[attributeLocationsStr];
    }

    // TODO: check if any attribute needs update

    // Bind vertex array
    glBindVertexArray(vao);

    // Draw
    if (indices_.empty()) {
        glDrawArrays(drawMode, 0, (GLsizei)attributes_["position"]->ItemCount());
    } else {
        glDrawElements(drawMode, (GLsizei)indices_.size(), GL_UNSIGNED_INT, 0);
    }

    // Unbind the vertex array
    glBindVertexArray(0);
}







} // namespace vivid