#ifndef MESH4D_HPP
#define MESH4D_HPP

#include <cassert>
#include <cstdint>
#include <fstream>
#include <vector>

#include "math.hpp"

static constexpr uint32_t MESH4D_MAGIC = 0x4D4D4D4D;
static constexpr uint32_t MESH4D_VERSION = 1;

struct Vertex4D {
    Vec4 position;
    Vec4 normal4D;
    Vec2 uv;
    uint32_t material = 0;
};

struct Tetrahedron {
    uint32_t a, b, c, d;
};

struct Triangle {
    uint32_t a, b, c;
};

struct Mesh4DHeader {
    uint32_t magic = MESH4D_MAGIC;
    uint32_t version = MESH4D_VERSION;
    uint32_t vertexCount = 0;
    uint32_t tetraCount = 0;
    uint32_t triangleCount = 0;
    uint32_t flags = 0;
};

struct Bounds4D {
    Vec4 min{};
    Vec4 max{};
};

class Mesh4D {
    std::vector<Vertex4D> vertices;
    std::vector<Tetrahedron> tetrahedra;
    std::vector<Triangle> surfaceTriangles;
    Bounds4D bounds{};

public:
    void computeBounds() {
        assert(!vertices.empty());

        bounds.min = bounds.max = vertices[0].position;
        for (const auto& v : vertices) {
            const Vec4& p = v.position;

            bounds.min.x = std::min(bounds.min.x, p.x);
            bounds.min.y = std::min(bounds.min.y, p.y);
            bounds.min.z = std::min(bounds.min.z, p.z);
            bounds.min.w = std::min(bounds.min.w, p.w);

            bounds.max.x = std::max(bounds.max.x, p.x);
            bounds.max.y = std::max(bounds.max.y, p.y);
            bounds.max.z = std::max(bounds.max.z, p.z);
            bounds.max.w = std::max(bounds.max.w, p.w);
        }
    }
};


#endif
