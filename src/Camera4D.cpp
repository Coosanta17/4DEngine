#include "../include/Camera4D.hpp"

Vec4 Camera4D::forward() const {
    const auto result = Vec4{
        std::cos(broach) * std::cos(pitch) * std::sin(yaw),
        std::cos(broach) * std::cos(pitch) * std::cos(yaw),
        std::cos(broach) * std::sin(pitch),
        std::sin(broach)
    };
    return result;
}

Vec4 Camera4D::right() const {
    return Vec4{
        std::cos(yaw),
        -std::sin(yaw),
        0.0f,
        0.0f
    };
}

Vec4 Camera4D::up() const {
    return Vec4{
        -std::sin(pitch) * std::sin(yaw),
        -std::sin(pitch) * std::cos(yaw),
        std::cos(pitch),
        0.0f
    };
}

Vec4 Camera4D::ana() const {
    return Vec4{
        -std::sin(broach) * std::cos(pitch) * std::sin(yaw),
        -std::sin(broach) * std::cos(pitch) * std::cos(yaw),
        -std::sin(broach) * std::sin(pitch),
        std::cos(broach)
    };
}

void Camera4D::moveForward() { applyMoveDelta(forward()); }
void Camera4D::moveBackward() { applyMoveDelta(-forward()); }
void Camera4D::moveRight() { applyMoveDelta(right()); }
void Camera4D::moveLeft() { applyMoveDelta(-right()); }
void Camera4D::moveUp() { applyMoveDelta(up()); }
void Camera4D::moveDown() { applyMoveDelta(-up()); }
void Camera4D::moveAta() { applyMoveDelta(ana()); }
void Camera4D::moveKata() { applyMoveDelta(-ana()); }

void Camera4D::lookLeft() { applyLookDelta(-1.0f, 0.0f, 0.0f); }
void Camera4D::lookRight() { applyLookDelta(1.0f, 0.0f, 0.0f); }
void Camera4D::lookUp() { applyLookDelta(0.0f, 1.0f, 0.0f); }
void Camera4D::lookDown() { applyLookDelta(0.0f, -1.0f, 0.0f); }
void Camera4D::lookAna() { applyLookDelta(0.0f, 0.0f, 0.1f); }
void Camera4D::lookKata() { applyLookDelta(0.0f, 0.0f, -0.1f); }

void Camera4D::deltaLook(const float& deltaX, const float& deltaY) {
    applyLookDelta(deltaX, deltaY, 0.0f);
}
