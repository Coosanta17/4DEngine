#ifndef CAMERA3D_HPP
#define CAMERA3D_HPP

#include <algorithm>
#include <iostream>
#include <numbers>
#include <ostream>

#include "Camera.hpp"

class Camera3D final : public Camera<3> {
    float& yaw = angles[0]; // xy plane (z-axis)
    float& pitch = angles[1]; // xz plane (y-axis)

protected:

public:
    // x=right, y=forward, z=up
    [[nodiscard]] Vec3 forward() const {
        const Vec3 result{cos(pitch) * sin(yaw), cos(pitch) * cos(yaw), sin(pitch)};
        return result.normalise();
    }

    [[nodiscard]] Vec3 right() const {
        return forward().cross(Vec3{0, 0, 1}).normalise();
    }

    // TODO: optimise unnecessary forward() calls before it becomes a problem
    [[nodiscard]] Vec3 up() const {
        return right().cross(forward()).normalise();
    }

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
        applyLookDelta(deltaX, deltaY);
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
