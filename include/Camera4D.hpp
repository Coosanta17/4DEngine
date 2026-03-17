#ifndef CAMERA4D_HPP
#define CAMERA4D_HPP

#include <cmath>

#include "Camera.hpp"

class Camera4D final : public Camera<4> {
    float& yaw = angles[0]; // xy plane
    float& pitch = angles[1]; // xz plane
    float& broach = angles[2]; // xw plane

protected:

public:
    // x=right, y=forward, z=up, w=ana.
    [[nodiscard]] Vec4 forward() const {
        const auto result = Vec4{
            cos(broach) * cos(pitch) * sin(yaw),
            cos(broach) * cos(pitch) * cos(yaw),
            cos(broach) * sin(pitch),
            sin(broach)
        };
        //std::cout << "forward: (" << result.x << ", " << result.y << ", " << result.z << ", " << result.w << ")" << std::endl;
        return result;
    }

    [[nodiscard]] Vec4 right() const {
        return Vec4{
            cos(yaw),
            -sin(yaw),
            0.0f,
            0.0f
        };
    }

    [[nodiscard]] Vec4 up() const {
        return Vec4{
            -sin(pitch) * sin(yaw),
            -sin(pitch) * cos(yaw),
            cos(pitch),
            0.0f
        };
    }

    [[nodiscard]] Vec4 ana() const {
        return Vec4{
            -sin(broach) * cos(pitch) * sin(yaw),
            -sin(broach) * cos(pitch) * cos(yaw),
            -sin(broach) * sin(pitch),
            cos(broach)
        };
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

    void moveAta() {
        applyMoveDelta(ana());
    }

    void moveKata() {
        applyMoveDelta(-ana());
    }

    void lookLeft() {
        applyLookDelta(-1.0f, 0.0f, 0.0f);
    }

    void lookRight() {
        applyLookDelta(1.0f, 0.0f, 0.0f);
    }

    void lookUp() {
        applyLookDelta(0.0f, 1.0f, 0.0f);
    }

    void lookDown() {
        applyLookDelta(0.0f, -1.0f, 0.0f);
    }

    void lookAna() {
        applyLookDelta(0.0f, 0.0f, 1.0f);
    }

    void lookKata() {
        applyLookDelta(0.0f, 0.0f, -1.0f);
    }

    void deltaLook(const float& deltaX, const float& deltaY) {
        applyLookDelta(deltaX, deltaY, 0.0f);
    }
};

#endif
