#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <algorithm>

#include "math.hpp"

constexpr float MAX_PITCH = (89 * std::numbers::pi_v<float>) / 180;

class Camera {
private:
    Vec3 pos{0, 0, 0.5};

    float yaw = 0; // z-axis
    float pitch = 0; // y-axis

    constexpr static float speed = 0.0005f;
    constexpr static float rotSpeed = 0.0005f;

public:
    void lookLeft() {
        yaw -= rotSpeed;
    }

    void lookRight() {
        yaw += rotSpeed;
    }

    void lookUp() {
        pitch += rotSpeed;
        pitch = std::clamp(pitch, -MAX_PITCH, MAX_PITCH);
    }

    void lookDown() {
        pitch -= rotSpeed;
        pitch = std::clamp(pitch, -MAX_PITCH, MAX_PITCH);
    }

    void deltaLook(const float& deltaX, const float& deltaY) {
        yaw -= deltaX * rotSpeed;
        pitch -= deltaY * rotSpeed;
        pitch = std::clamp(pitch, -MAX_PITCH, MAX_PITCH);
    }

    [[nodiscard]] float getYaw() const {
        return this->yaw;
    }

    [[nodiscard]] float getPitch() const {
        return this->pitch;
    }

    [[nodiscard]] const Vec3& getPos() const {
        return this->pos;
    }

    [[nodiscard]] Vec3 forward() const {
        const Vec3 result{std::cos(pitch) * std::cos(yaw), std::cos(pitch) * std::sin(yaw), std::sin(pitch)};
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
        pos += forward() * speed;
    }

    void moveBackward() {
        pos -= forward() * speed;
    }

    void moveRight() {
        pos += right() * speed;
    }

    void moveLeft() {
        pos -= right() * speed;
    }

    void moveUp() {
        pos += up() * speed;
    }

    void moveDown() {
        pos -= up() * speed;
    }
};

#endif
