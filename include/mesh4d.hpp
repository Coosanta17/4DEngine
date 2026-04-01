#ifndef MESH4D_HPP
#define MESH4D_HPP

#include <cassert>
#include <cstdint>
#include <fstream>
#include <vector>

#include "mesh4d_generated.h"

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

struct SerialisedMesh4D {
    uint8_t* buf;
    size_t size;
};

class IMesh4D {
protected:
    Mesh4DHeader header;
    std::vector<Vertex4D> vertices;
    std::vector<Tetrahedron> tetrahedra;
    std::vector<Triangle> surfaceTriangles;
    Bounds4D bounds{};

public:
    virtual ~IMesh4D() = default;
    virtual Mesh4DHeader getHeader();

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

class Mesh4D {
    const mesh4d_fbs::Mesh4D* fb_mesh = nullptr;

protected:
    Mesh4DHeader header;
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

    SerialisedMesh4D serialise() {
        const u_int64_t size = sizeof(Mesh4DHeader) +
            vertices.size() * sizeof(Vertex4D) +
            tetrahedra.size() * sizeof(Tetrahedron) +
            surfaceTriangles.size() * sizeof(Triangle) +
            sizeof(Bounds4D) + 1024;
        flatbuffers::FlatBufferBuilder builder(size);

        const auto header_offset = mesh4d_fbs::CreateMesh4DHeader(
            builder,
            header.magic,
            header.version,
            header.vertexCount,
            header.tetraCount,
            header.triangleCount,
            header.flags
        );

        // Convert structs to fbs versions

        std::vector<mesh4d_fbs::Vertex4D> fb_vertices;
        fb_vertices.reserve(vertices.size());
        for (const auto& [position, normal4D, uv, material] : vertices) {
            fb_vertices.emplace_back(
                mesh4d_fbs::Vec4(position.x, position.y, position.z, position.w),
                mesh4d_fbs::Vec4(normal4D.x, normal4D.y, normal4D.z, normal4D.w),
                mesh4d_fbs::Vec2(uv.x, uv.y),
                material
            );
        }
        const auto vertices_offset = builder.CreateVectorOfStructs(fb_vertices.data(), fb_vertices.size());

        std::vector<mesh4d_fbs::Tetrahedron> fb_tetrahedra;
        fb_tetrahedra.reserve(tetrahedra.size());
        for (const auto& [a, b, c, d] : tetrahedra) {
            fb_tetrahedra.emplace_back(a, b, c, d);
        }
        const auto tetrahedra_offset = builder.CreateVectorOfStructs(fb_tetrahedra.data(), fb_tetrahedra.size());

        std::vector<mesh4d_fbs::Triangle> fb_triangles;
        fb_triangles.reserve(surfaceTriangles.size());
        for (const auto& [a, b, c] : surfaceTriangles) {
            fb_triangles.emplace_back(a, b, c);
        }
        const auto triangle_offset = builder.CreateVectorOfStructs(fb_triangles.data(), fb_triangles.size());

        const mesh4d_fbs::Bounds4D fb_bounds(
            mesh4d_fbs::Vec4(bounds.min.x, bounds.min.y, bounds.min.z, bounds.min.w),
            mesh4d_fbs::Vec4(bounds.max.x, bounds.max.y, bounds.max.z, bounds.max.w)
        );

        // serialise

        const auto mesh_offset = mesh4d_fbs::CreateMesh4D(
            builder,
            header_offset,
            vertices_offset,
            tetrahedra_offset,
            triangle_offset,
            &fb_bounds
        );

        builder.Finish(mesh_offset);

        return {builder.GetBufferPointer(), builder.GetSize()};
    }
};

class ImmutableMesh4D : Mesh4D {
    std::shared_ptr<std::vector<uint8_t>> buffer; // To keep buffer alive
    const mesh4d_fbs::Mesh4D* fb_mesh = nullptr;

public:
    explicit ImmutableMesh4D(SerialisedMesh4D& serialised_mesh) {
        this->buffer = serialised_mesh.buf;
        this->fb_mesh = mesh4d_fbs::GetMesh4D(this->buffer->data());
    }
};

#endif
