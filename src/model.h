#pragma once

#include <vector>

#include "vec3.h"

class Model final
{
public:
    Model();
    ~Model();

    bool Load(const char* filename);
    uint32_t GetNumVerts() const { return static_cast<uint32_t>(m_verts.size()); }
    uint32_t GetNumFaces() const { return static_cast<uint32_t>(m_faces.size()); }
    const Vec3f& GetVert(uint32_t i) const { return m_verts[i]; }
    const std::vector<uint32_t>& GetFace(uint32_t i) const { return m_faces[i]; }

private:
    std::vector<Vec3f> m_verts{};
    std::vector<std::vector<uint32_t>> m_faces{};
};
