#pragma once

#include <iostream>
#include <utility>
#include <vector>
#include <map>

namespace vivid {

constexpr int kAttribNum = 11;

enum AttributeType : int {
    Position = 0,
    Normal = 1,
    Tangent = 2,
    Color = 3,
    TexCoord0 = 4,
    TexCoord1 = 5,
    TexCoord2 = 6,
    TexCoord3 = 7,
    TexCoord4 = 8,
    BlendWeight = 9,
    BlendIndices = 10
};

static std::string AttributeName(const AttributeType& type) {
    switch (type) {
        case Position: return "position";
        case Normal: return "normal";
        case Tangent: return "tangent";
        case Color: return "color";
        case TexCoord0: return "texCoord0";
        case TexCoord1: return "texCoord1";
        case TexCoord2: return "texCoord2";
        case TexCoord3: return "texCoord3";
        case TexCoord4: return "texCoord4";
        case BlendWeight: return "blendWeight";
        case BlendIndices: return "blendIndices";
        default: return "unknown";
    }
};


/* Element: a single number.
 * Item: a struct composed of several numbers. For example, a 3D point is an item composed of 3 elements: x, y, z .
 */
class Attribute {
public:
    Attribute(AttributeType type, int elementsPerItem, bool normalized, const std::vector<float> *data = nullptr)
        : type_(std::move(type)), elementsPerItem_(elementsPerItem), normalized_(normalized)
    {
        // Derive name from type
        name_ = AttributeName(type);
        if (data != nullptr) {
            data_ = (*data);
        }
    }

    inline AttributeType Type() const { return type_; }

    inline std::string Name() const { return name_; }

    // Number of elements in each item
    inline int ElementsPerItem() const { return elementsPerItem_; }

    // Is normalized or not?
    inline bool Normalized() const { return normalized_; }

    // Get the number of items in this attribute
    inline int ItemCount() const {
        return static_cast<int>(data_.size()) / elementsPerItem_;
    }

    // Get the number of elements
    inline int ElementCount() const {
        return static_cast<int>(data_.size());
    }

    inline size_t ItemSize() const {
        return elementsPerItem_ * sizeof(float);
    }

    inline size_t TotalSize() const {
        return data_.size() * sizeof(float);
    }

    inline void SetData(std::vector<float> &data, bool useMove = false) {
        data_ = useMove ? std::move(data) : data;
    }

    inline const std::vector<float>& GetData() const {
        return data_;
    }

    // Vertex buffer object handle
    inline unsigned int VBO() const { return vbo_; }
    inline void SetVBO(unsigned int vbo) { vbo_ = vbo; }

private:
    AttributeType type_;
    int elementsPerItem_;
    bool normalized_;

    std::string name_;

    std::vector<float> data_;

    // Vertex buffer object handle
    unsigned int vbo_ = 0;
};

using AttributePtr = std::shared_ptr<Attribute>;

} // namespace vivid