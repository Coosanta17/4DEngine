#ifndef MESH4D_HPP
#define MESH4D_HPP

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <vector>

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>

#include "math.hpp"

static constexpr uint32_t MESH4D_MAGIC = 0x4DE004DE;
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

template <class Archive, int N>
void save(Archive& ar, const Vector<N>& v) {
    ar(cereal::binary_data(v.data, sizeof(v.data)));
}

template <class Archive, int N>
void load(Archive& ar, Vector<N>& v) {
    ar(cereal::binary_data(v.data, sizeof(v.data)));
}

template <class Archive>
void serialize(Archive& ar, Vertex4D& v) {
    ar(v.position, v.normal4D, v.uv, v.material);
}

template <class Archive>
void serialize(Archive& ar, Tetrahedron& t) {
    ar(t.a, t.b, t.c, t.d);
}

template <class Archive>
void serialize(Archive& ar, Triangle& t) {
    ar(t.a, t.b, t.c);
}

template <class Archive>
void serialize(Archive& ar, Mesh4DHeader& h) {
    ar(h.magic, h.version, h.vertexCount, h.tetraCount, h.triangleCount, h.flags);
}

template <class Archive>
void serialize(Archive& ar, Bounds4D& b) {
    ar(b.min, b.max);
}

class Mesh4D {
protected:
    Mesh4DHeader header;
    std::vector<Vertex4D> vertices;
    std::vector<Tetrahedron> tetrahedra;
    std::vector<Triangle> surfaceTriangles;
    Bounds4D bounds{};

    void syncHeader() noexcept {
        header.magic = MESH4D_MAGIC;
        header.version = MESH4D_VERSION;
        header.vertexCount = static_cast<uint32_t>(vertices.size());
        header.tetraCount = static_cast<uint32_t>(tetrahedra.size());
        header.triangleCount = static_cast<uint32_t>(surfaceTriangles.size());
    }

public:
    [[nodiscard]] const Mesh4DHeader& getHeader() const noexcept { return header; }
    [[nodiscard]] const Bounds4D& getBounds() const noexcept { return bounds; }
    [[nodiscard]] const std::vector<Vertex4D>& getVertices() const noexcept { return vertices; }
    [[nodiscard]] std::vector<Vertex4D>& getVertices() noexcept { return vertices; }
    [[nodiscard]] const std::vector<Tetrahedron>& getTetrahedra() const noexcept { return tetrahedra; }
    [[nodiscard]] std::vector<Tetrahedron>& getTetrahedra() noexcept { return tetrahedra; }
    [[nodiscard]] const std::vector<Triangle>& getSurfaceTriangles() const noexcept { return surfaceTriangles; }
    [[nodiscard]] std::vector<Triangle>& getSurfaceTriangles() noexcept { return surfaceTriangles; }

    [[nodiscard]] bool empty() const noexcept {
        return vertices.empty() && tetrahedra.empty() && surfaceTriangles.empty();
    }

    void clear() noexcept {
        vertices.clear();
        tetrahedra.clear();
        surfaceTriangles.clear();
        bounds = {};
        header = {};
        syncHeader();
    }

    void reserve(const std::size_t vertexCapacity, const std::size_t tetraCapacity = 0,
                 const std::size_t triangleCapacity = 0) {
        vertices.reserve(vertexCapacity);
        tetrahedra.reserve(tetraCapacity);
        surfaceTriangles.reserve(triangleCapacity);
    }

    void refreshHeader() noexcept {
        syncHeader();
    }

    [[nodiscard]] bool validate() const noexcept {
        return header.magic == MESH4D_MAGIC &&
            header.version == MESH4D_VERSION &&
            header.vertexCount == vertices.size() &&
            header.tetraCount == tetrahedra.size() &&
            header.triangleCount == surfaceTriangles.size();
    }

    Vertex4D& addVertex(const Vertex4D& vertex) {
        vertices.push_back(vertex);

        if (vertices.size() == 1) {
            bounds.min = bounds.max = vertex.position;
        }
        else {
            const Vec4& p = vertex.position;

            bounds.min.x = std::min(bounds.min.x, p.x);
            bounds.min.y = std::min(bounds.min.y, p.y);
            bounds.min.z = std::min(bounds.min.z, p.z);
            bounds.min.w = std::min(bounds.min.w, p.w);

            bounds.max.x = std::max(bounds.max.x, p.x);
            bounds.max.y = std::max(bounds.max.y, p.y);
            bounds.max.z = std::max(bounds.max.z, p.z);
            bounds.max.w = std::max(bounds.max.w, p.w);
        }

        header.vertexCount = static_cast<uint32_t>(vertices.size());
        return vertices.back();
    }

    Vertex4D& addVertex(const Vec4& position, const Vec4& normal4D = {}, const Vec2& uv = {},
                        const uint32_t material = 0) {
        return addVertex(Vertex4D{position, normal4D, uv, material});
    }

    Tetrahedron& addTetrahedron(const uint32_t a, const uint32_t b, const uint32_t c, const uint32_t d) {
        tetrahedra.push_back({a, b, c, d});
        header.tetraCount = static_cast<uint32_t>(tetrahedra.size());
        return tetrahedra.back();
    }

    Triangle& addTriangle(const uint32_t a, const uint32_t b, const uint32_t c) {
        surfaceTriangles.push_back({a, b, c});
        header.triangleCount = static_cast<uint32_t>(surfaceTriangles.size());
        return surfaceTriangles.back();
    }

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

    template <class Archive>
    void serialise(Archive& ar) {
        ar(header, bounds, vertices, tetrahedra, surfaceTriangles);
    }

    template <class Archive>
    void save(Archive& ar) const {
        auto* self = const_cast<Mesh4D*>(this);
        self->syncHeader();
        self->serialise(ar);
    }

    template <class Archive>
    void load(Archive& ar) {
        serialise(ar);

        if (header.magic != MESH4D_MAGIC || header.version != MESH4D_VERSION) {
            throw cereal::Exception("Invalid Mesh4D header");
        }

        if (header.vertexCount != vertices.size() ||
            header.tetraCount != tetrahedra.size() ||
            header.triangleCount != surfaceTriangles.size()) {
            throw cereal::Exception("Mesh4D header counts do not match payload");
        }

        syncHeader();
    }
};

#endif
