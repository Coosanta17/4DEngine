#ifndef MESH4D_HPP
#define MESH4D_HPP

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <vector>
#include <string>
#include <iostream>

#include <cereal/cereal.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/portable_binary.hpp>

#include "math.hpp"

static constexpr uint32_t MESH4D_MAGIC = 0x4DE4DE;
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

    void syncHeader() noexcept;

public:
    [[nodiscard]] const Mesh4DHeader& getHeader() const noexcept { return header; }
    [[nodiscard]] const Bounds4D& getBounds() const noexcept { return bounds; }
    [[nodiscard]] const std::vector<Vertex4D>& getVertices() const noexcept { return vertices; }
    [[nodiscard]] std::vector<Vertex4D>& getVertices() noexcept { return vertices; }
    [[nodiscard]] const std::vector<Tetrahedron>& getTetrahedra() const noexcept { return tetrahedra; }
    [[nodiscard]] std::vector<Tetrahedron>& getTetrahedra() noexcept { return tetrahedra; }
    [[nodiscard]] const std::vector<Triangle>& getSurfaceTriangles() const noexcept { return surfaceTriangles; }
    [[nodiscard]] std::vector<Triangle>& getSurfaceTriangles() noexcept { return surfaceTriangles; }

    [[nodiscard]] bool empty() const noexcept;
    void clear() noexcept;

    void reserve(std::size_t vertexCapacity, std::size_t tetraCapacity = 0,
                 std::size_t triangleCapacity = 0);

    void refreshHeader() noexcept;

    [[nodiscard]] bool validate() const noexcept;

    Vertex4D& addVertex(const Vertex4D& vertex);
    Vertex4D& addVertex(const Vec4& position, const Vec4& normal4D = {}, const Vec2& uv = {},
                        uint32_t material = 0);

    Tetrahedron& addTetrahedron(uint32_t a, uint32_t b, uint32_t c, uint32_t d);
    Triangle& addTriangle(uint32_t a, uint32_t b, uint32_t c);
    
    void adjust_bounds(const Vertex4D& vertex);
    void computeBounds();

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

    [[nodiscard]] bool writeToFile(const std::string& filepath) const noexcept {
        try {
            std::ofstream os(filepath, std::ios::binary);
            if (!os.is_open()) {
                std::cerr << "Mesh4D Error: Failed to open path for writing: " << filepath << "\n";
                return false;
            }

            // Enclosing archive initialization forces local block destruction
            // to flush all header/footer serialization tracking info to disk safely.
            {
                cereal::PortableBinaryOutputArchive archive(os);
                archive(*this);
            }
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Mesh4D Write Exception: " << e.what() << "\n";
            return false;
        }
    }

    bool readFromFile(const std::string& filepath) noexcept {
        try {
            std::ifstream is(filepath, std::ios::binary);
            if (!is.is_open()) {
                std::cerr << "Mesh4D Error: Failed to open file for reading: " << filepath << "\n";
                return false;
            }

            {
                cereal::PortableBinaryInputArchive archive(is);
                archive(*this);
            }
            return true;
        }
        catch (const std::exception& e) {
            std::cerr << "Mesh4D Read Exception: " << e.what() << "\n";
            return false;
        }
    }
};

#endif
