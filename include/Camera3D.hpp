#ifndef CAMERA3D_HPP
#define CAMERA3D_HPP

#include <algorithm>
#include <numbers>

#include "Camera.hpp"

class Camera3D final : public Camera<3> {
    float& yaw = angles[0]; // xy plane (z-axis)
    float& pitch = angles[1]; // xz plane (y-axis)

protected:

public:
    void lookLeft() {
        applyLookDelta(-1.0f, 0.0f);
    }

    void lookRight() {
        applyLookDelta(1.0f, 0.0f);
    }

    void lookUp() {
        applyLookDelta(0.0f, 1.0f);
    }

    void lookDown() {
        applyLookDelta(0.0f, -1.0f);
    }

    void deltaLook(const float& deltaX, const float& deltaY) {
        applyLookDelta(-deltaX, -deltaY);
    }

    [[nodiscard]] Vec3 forward() const {
        const Vec3 result{cos(pitch) * cos(yaw), cos(pitch) * sin(yaw), sin(pitch)};
        return result.normalise();
    }

    [[nodiscard]] Vec3 right() const {
        return forward().cross(Vec3{0, 0, 1}).normalise();
    }

    // TODO: optimise unnecessary forward() calls before it becomes a problem
    [[nodiscard]] Vec3 up() const {
        return right().cross(forward()).normalise();
    }

    void moveForward() {
        applyMoveDelta(forward());
    }

    void moveBackward() {
        applyMoveDelta(-forward());
    }

    void moveRight() {
        applyMoveDelta(right());
    }

    void moveLeft() {
        applyMoveDelta(-right());
    }

    void moveUp() {
        applyMoveDelta(up());
    }

    void moveDown() {
        applyMoveDelta(-up());
    }
};

#endif
