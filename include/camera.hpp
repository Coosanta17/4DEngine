#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "math.hpp"

struct Camera {
    Vec3 pos{0, 0, 0.5};

    float yaw = 0; // z-axis
    float pitch = 0; // y-axis

    [[nodiscard]] Vec3 forward() const {
        const Vec3 result{std::cos(yaw) * std::cos(pitch), std::sin(pitch), std::sin(yaw) * std::cos(pitch)};
        return result.normalise();
    }

    [[nodiscard]] Vec3 right() const {
        return forward().cross(Vec3{0,1,0}).normalise();
    }

    // TODO: optimise unnecessary forward() calls before it becomes a problem
    [[nodiscard]] Vec3 up() const {
        return right().cross(forward()).normalise();
    }
};

#endif
