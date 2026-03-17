#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <cmath>

#include <SDL2/SDL.h>

#include "Camera3D.hpp"
#include "Camera4D.hpp"
#include "math.hpp"
#include "mesh.hpp"

struct Renderer3D {
    SDL_Renderer* sdlRenderer;
    int width;
    int height;

    static constexpr float nearPlane = 0.01f;

    [[nodiscard]] static bool clipSegmentToNearPlane(Vec3& v1, Vec3& v2, const float nearY) noexcept {
        const bool v1Inside = v1.y >= nearY;
        const bool v2Inside = v2.y >= nearY;

        if (!v1Inside && !v2Inside) {
            return false;
        }

        if (v1Inside && v2Inside) {
            return true;
        }

        const Vec3 delta = v2 - v1;
        if (constexpr float epsilon = 1e-6f; std::fabs(delta.y) <= epsilon) {
            return false;
        }

        float t = (nearY - v1.y) / delta.y;
        if (t < 0.0f) {
            t = 0.0f;
        }
        else if (t > 1.0f) {
            t = 1.0f;
        }

        const Vec3 intersection = v1 + delta * t;
        if (!v1Inside) {
            v1 = intersection;
        }
        else {
            v2 = intersection;
        }

        return true;
    }

    [[nodiscard]] Vec3 project(const Vec3& p) const noexcept {
        constexpr float fov = 400.0f;
        const float x = (p.x / p.y) * fov + static_cast<float>(width) / 2.0f;
        const float y = -(p.z / p.y) * fov + static_cast<float>(height) / 2.0f;
        return Vec3{x, y, p.y};
    }

    void drawCameraSpaceSegment(Vec3 v1, Vec3 v2) const {
        if (!clipSegmentToNearPlane(v1, v2, nearPlane)) {
            return;
        }

        const Vec3 p1 = project(v1);
        const Vec3 p2 = project(v2);

        SDL_RenderDrawLine(
            sdlRenderer,
            static_cast<int>(p1.x),
            static_cast<int>(p1.y),
            static_cast<int>(p2.x),
            static_cast<int>(p2.y)
        );
    }

    void drawCameraSpaceMesh(const Mesh<3>& mesh) const {
        for (const auto& [a, b] : mesh.lines) {
            drawCameraSpaceSegment(mesh.vertices[a], mesh.vertices[b]);
        }
    }

    void drawMesh(const Mesh<3>& mesh, const Camera3D& cam) const {
        const Vec3 forward = cam.forward();
        const Vec3 right = cam.right();
        const Vec3 up = cam.up();

        for (const auto& [a, b] : mesh.lines) {
            Vec3 v1 = mesh.vertices[a] - cam.getPos();
            Vec3 v2 = mesh.vertices[b] - cam.getPos();

            // x=right, y=forward, z=up.
            v1 = Vec3{v1.dot(right), v1.dot(forward), v1.dot(up)};
            v2 = Vec3{v2.dot(right), v2.dot(forward), v2.dot(up)};
            drawCameraSpaceSegment(v1, v2);
        }
    }
};

struct Renderer4D {
    SDL_Renderer* sdlRenderer;
    int width;
    int height;

    static constexpr float nearPlane4D = 0.01f;
    static constexpr float fov4D = 1.0f;

    [[nodiscard]] static bool clipSegmentToNearPlane4D(Vec4& v1, Vec4& v2, const float nearW) noexcept {
        const bool v1Inside = v1.w >= nearW;
        const bool v2Inside = v2.w >= nearW;

        if (!v1Inside && !v2Inside) {
            return false;
        }

        if (v1Inside && v2Inside) {
            return true;
        }

        const Vec4 delta = v2 - v1;
        if (constexpr float epsilon = 1e-6f; std::fabs(delta.w) <= epsilon) {
            return false;
        }

        float t = (nearW - v1.w) / delta.w;
        if (t < 0.0f) {
            t = 0.0f;
        }
        else if (t > 1.0f) {
            t = 1.0f;
        }

        const Vec4 intersection = v1 + delta * t;
        if (!v1Inside) {
            v1 = intersection;
        }
        else {
            v2 = intersection;
        }

        return true;
    }

    [[nodiscard]] static Vec4 toCameraSpace(const Vec4& point, const Camera4D& cam) noexcept {
        const Vec4 relative = point - cam.getPos();

        // x=right, y=forward, z=up, w=ana.
        return Vec4{
            relative.dot(cam.right()),
            relative.dot(cam.forward()),
            relative.dot(cam.up()),
            relative.dot(cam.ana())
        };
    }

    [[nodiscard]] static Vec3 project4Dto3D(const Vec4& p) noexcept {
        const float wScale = fov4D / p.w;
        return Vec3{p.x * wScale, p.y, p.z * wScale};
    }

    static void appendProjectedLine(Mesh<3>& outMesh, const Vec3& a, const Vec3& b) {
        const int start = static_cast<int>(outMesh.vertices.size());
        outMesh.vertices.push_back(a);
        outMesh.vertices.push_back(b);
        outMesh.lines.push_back({start, start + 1});
    }

    [[nodiscard]] static Mesh<3> projectMeshToCameraSpace3D(const Mesh<4>& mesh, const Camera4D& cam) {
        Mesh<3> projected{};
        projected.vertices.reserve(mesh.lines.size() * 2);
        projected.lines.reserve(mesh.lines.size());

        for (const auto& [a, b] : mesh.lines) {
            Vec4 v1 = toCameraSpace(mesh.vertices[a], cam);
            Vec4 v2 = toCameraSpace(mesh.vertices[b], cam);

            if (!clipSegmentToNearPlane4D(v1, v2, nearPlane4D)) {
                continue;
            }

            appendProjectedLine(projected, project4Dto3D(v1), project4Dto3D(v2));
        }

        return projected;
    }

    void drawMesh(const Mesh<4>& mesh, const Camera4D& cam) const {
        const Mesh<3> projected = projectMeshToCameraSpace3D(mesh, cam);
        const Renderer3D renderer3D{sdlRenderer, width, height};
        renderer3D.drawCameraSpaceMesh(projected);
    }
};

#endif
