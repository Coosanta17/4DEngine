#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL2/SDL.h>

#include "Camera3D.hpp"
#include "math.hpp"
#include "mesh.hpp"

struct Renderer {
    SDL_Renderer* sdlRenderer;
    int width;
    int height;

    [[nodiscard]] Vec3 project(const Vec3& p) const noexcept {
        constexpr float fov = 400.0f;
        const float x = (p.x / p.z) * fov + static_cast<float>(width) / 2.0f;
        const float y = -(p.y / p.z) * fov + static_cast<float>(height) / 2.0f;
        return Vec3{x, y, p.z};
    }

    void drawMesh(const Mesh& mesh, const Camera3D& cam) const {
        const Vec3 forward = cam.forward();
        const Vec3 right = cam.right();
        const Vec3 up = cam.up();

        for (const auto& [a, b] : mesh.lines) {
            Vec3 v1 = mesh.vertices[a] - cam.getPos();
            Vec3 v2 = mesh.vertices[b] - cam.getPos();

            // Convert world-space points to camera-space using camera basis vectors.
            v1 = Vec3{v1.dot(right), v1.dot(up), v1.dot(forward)};
            v2 = Vec3{v2.dot(right), v2.dot(up), v2.dot(forward)};

            if (v1.z <= 0.0f || v2.z <= 0.0f) {
                continue;
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
    }
};

#endif
