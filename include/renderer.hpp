#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <cmath>

#include <SDL3/SDL_render.h>

#include "Camera3D.hpp"
#include "Camera4D.hpp"
#include "math.hpp"
#include "mesh.hpp"
#include "mesh4d.hpp"

struct Renderer3D {
    SDL_Renderer* sdlRenderer;
    int width;
    int height;

    static constexpr float nearPlane = 0.01f;

    [[nodiscard]] static bool clipSegmentToNearPlane(Vec3& v1, Vec3& v2, float nearY) noexcept;
    [[nodiscard]] Vec3 project(const Vec3& p) const noexcept;
    void drawCameraSpaceSegment(Vec3 v1, Vec3 v2) const;
    void drawCameraSpaceMesh(const Mesh<3>& mesh) const;
    void drawMesh(const Mesh<3>& mesh, const Camera3D& cam) const;
};

struct Renderer4D {
    SDL_Renderer* sdlRenderer;
    int width;
    int height;

    static constexpr float nearPlane4D = 0.01f;
    static constexpr float fov4D = 1.0f;

    [[nodiscard]] static bool clipSegmentToNearPlane4D(Vec4& v1, Vec4& v2, float nearW) noexcept;
    [[nodiscard]] static Vec4 toCameraSpace(const Vec4& point, const Camera4D& cam) noexcept;
    [[nodiscard]] static Vec3 project4Dto3D(const Vec4& p) noexcept;
    static void appendProjectedLine(Mesh<3>& outMesh, const Vec3& a, const Vec3& b);
    [[nodiscard]] static Mesh<3> projectMeshToCameraSpace3D(const Mesh<4>& mesh, const Camera4D& cam);
    [[nodiscard]] static Mesh<3> projectMesh4DToCameraSpace3D(const Mesh4D& mesh, const Camera4D& cam);

    void drawMesh(const Mesh<4>& mesh, const Camera4D& cam) const;
    void drawMesh(const Mesh4D& mesh, const Camera4D& cam) const;
};

#endif
