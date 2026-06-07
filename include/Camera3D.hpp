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
    [[nodiscard]] Vec3 forward() const;
    [[nodiscard]] Vec3 right() const;
    [[nodiscard]] Vec3 up() const;

    void lookLeft();
    void lookRight();
    void lookUp();
    void lookDown();
    void deltaLook(const float& deltaX, const float& deltaY);

    void moveForward();
    void moveBackward();
    void moveRight();
    void moveLeft();
    void moveUp();
    void moveDown();
};

#endif
