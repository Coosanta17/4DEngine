#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <span>

#include "math.hpp"

constexpr float MAX_PITCH = std::numbers::pi_v<float> / 2 - 0.0001f;

// Camera Core for N-Dimensional cameras, where N > 2 (well technically N can be 2, but I have no idea what it would do)
template <int N>
class Camera {
protected:
    Vector<N> pos{};
    float angles[N]{};

    constexpr static float moveSpeed = 0.0005f;
    constexpr static float lookSpeed = 0.0025f;

    // TODO Decide between quaternions or matrices instead of whatever this is.
    template <typename... Args>
    void applyLookDelta(Args&&... args) {
        static_assert(sizeof...(args) == N - 1, "Number of deltas must match N-1");
        float deltas[N]{static_cast<float>(args)...};

        for (int i = 0; i < N; ++i) {
            angles[i] += deltas[i] * lookSpeed;
        }

        for (int i = 1; i < N; ++i) {
            angles[i] = std::clamp(angles[i], -MAX_PITCH, MAX_PITCH);
        }
    }

    void applyMoveDelta(const Vector<N>& delta) {
        pos += delta * moveSpeed;
        for (int i = 0; i < N; ++i) {
            std::cout << pos[i] << (i < N - 1 ? ", " : "\n");
        }
    }

public:
    virtual ~Camera() = default;

    [[nodiscard]] const Vector<N>& getPos() const {
        return this->pos;
    }
};

#endif
