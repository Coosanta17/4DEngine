#include "../include/mesh4d.hpp"

void Mesh4D::syncHeader() noexcept {
    header.magic = MESH4D_MAGIC;
    header.version = MESH4D_VERSION;
    header.vertexCount = static_cast<uint32_t>(vertices.size());
    header.tetraCount = static_cast<uint32_t>(tetrahedra.size());
    header.triangleCount = static_cast<uint32_t>(surfaceTriangles.size());
}

bool Mesh4D::empty() const noexcept {
    return vertices.empty() && tetrahedra.empty() && surfaceTriangles.empty();
}

void Mesh4D::clear() noexcept {
    vertices.clear();
    tetrahedra.clear();
    surfaceTriangles.clear();
    bounds = {};
    header = {};
    syncHeader();
}

void Mesh4D::reserve(std::size_t vertexCapacity, std::size_t tetraCapacity,
                     std::size_t triangleCapacity) {
    vertices.reserve(vertexCapacity);
    tetrahedra.reserve(tetraCapacity);
    surfaceTriangles.reserve(triangleCapacity);
}

void Mesh4D::refreshHeader() noexcept {
    syncHeader();
}

bool Mesh4D::validate() const noexcept {
    return header.magic == MESH4D_MAGIC &&
        header.version == MESH4D_VERSION &&
        header.vertexCount == vertices.size() &&
        header.tetraCount == tetrahedra.size() &&
        header.triangleCount == surfaceTriangles.size();
}

void Mesh4D::adjust_bounds(const Vertex4D& vertex) {
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

Vertex4D& Mesh4D::addVertex(const Vertex4D& vertex) {
    vertices.push_back(vertex);

    if (vertices.size() == 1) {
        bounds.min = bounds.max = vertex.position;
    }
    else {
        adjust_bounds(vertex);
    }

    header.vertexCount = static_cast<uint32_t>(vertices.size());
    return vertices.back();
}

Vertex4D& Mesh4D::addVertex(const Vec4& position, const Vec4& normal4D, const Vec2& uv,
                            const uint32_t material) {
    return addVertex(Vertex4D{position, normal4D, uv, material});
}

Tetrahedron& Mesh4D::addTetrahedron(const uint32_t a, const uint32_t b, const uint32_t c, const uint32_t d) {
    tetrahedra.push_back({a, b, c, d});
    header.tetraCount = static_cast<uint32_t>(tetrahedra.size());
    return tetrahedra.back();
}

Triangle& Mesh4D::addTriangle(const uint32_t a, const uint32_t b, const uint32_t c) {
    surfaceTriangles.push_back({a, b, c});
    header.triangleCount = static_cast<uint32_t>(surfaceTriangles.size());
    return surfaceTriangles.back();
}

void Mesh4D::computeBounds() {
    assert(!vertices.empty());

    bounds.min = bounds.max = vertices[0].position;
    for (const auto& v : vertices) {
        adjust_bounds(v);
    }
}
