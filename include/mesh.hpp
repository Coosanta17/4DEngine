#ifndef MESH_HPP
#define MESH_HPP

#include <vector>
#include "math.hpp"

struct Line {
    int a, b;
};

struct Mesh {
    std::vector<Vec3> vertices;
    std::vector<Line> lines;

    static Mesh makeCube() {
        Mesh m;

        m.vertices = {
            Vec3{-1, -1, -1}, Vec3{1, -1, -1}, Vec3{1, 1, -1}, Vec3{-1, 1, -1},
            Vec3{-1, -1, 1}, Vec3{1, -1, 1}, Vec3{1, 1, 1}, Vec3{-1, 1, 1}
        };

        m.lines = {
            {0, 1}, {1, 2}, {2, 3}, {3, 0},
            {4, 5}, {5, 6}, {6, 7}, {7, 4},
            {0, 4}, {1, 5}, {2, 6}, {3, 7}
        };

        return m;
    }

    static Mesh makeAxes(const float length = 2.0f) {
        Mesh m;
        m.vertices = {
            Vec3{0, 0, 0}, // origin
            // Vec3{length, 0, 0}, // +X
            // Vec3{0, length, 0}, // +Y
            Vec3{0, 0, length} // +Z
        };
        m.lines = {
            // {0, 1}, // X axis
            // {0, 2}, // Y axis
            {0, 1} // Z axis
        };
        return m;
    }
};

#endif
