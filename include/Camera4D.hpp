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
    [[nodiscard]] Vec4 forward() const;
    [[nodiscard]] Vec4 right() const;
    [[nodiscard]] Vec4 up() const;
    [[nodiscard]] Vec4 ana() const;

    void moveForward();
    void moveBackward();
    void moveRight();
    void moveLeft();
    void moveUp();
    void moveDown();
    void moveAta();
    void moveKata();

    void lookLeft();
    void lookRight();
    void lookUp();
    void lookDown();
    void lookAna();
    void lookKata();

    void deltaLook(const float& deltaX, const float& deltaY);
};

#endif
