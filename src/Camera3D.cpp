#include "../include/Camera3D.hpp"
#include <cmath>

Vec3 Camera3D::forward() const {
    const Vec3 result{std::cos(pitch) * std::sin(yaw), std::cos(pitch) * std::cos(yaw), std::sin(pitch)};
    return result.normalise();
}

Vec3 Camera3D::right() const {
    return forward().cross(Vec3{0, 0, 1}).normalise();
}

Vec3 Camera3D::up() const {
    return right().cross(forward()).normalise();
}

void Camera3D::lookLeft() { applyLookDelta(-1.0f, 0.0f); }
void Camera3D::lookRight() { applyLookDelta(1.0f, 0.0f); }
void Camera3D::lookUp() { applyLookDelta(0.0f, 1.0f); }
void Camera3D::lookDown() { applyLookDelta(0.0f, -1.0f); }

void Camera3D::deltaLook(const float& deltaX, const float& deltaY) {
    applyLookDelta(deltaX, deltaY);
}

void Camera3D::moveForward() { applyMoveDelta(forward()); }
void Camera3D::moveBackward() { applyMoveDelta(-forward()); }
void Camera3D::moveRight() { applyMoveDelta(right()); }
void Camera3D::moveLeft() { applyMoveDelta(-right()); }
void Camera3D::moveUp() { applyMoveDelta(up()); }
void Camera3D::moveDown() { applyMoveDelta(-up()); }
