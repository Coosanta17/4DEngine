#ifndef MESH_HPP
#define MESH_HPP

#include <cstddef>
#include <vector>
#include "math.hpp"

struct Line {
    int a, b;
};

template <int N>
struct Mesh {
    static_assert(N > 0, "Mesh dimension must be positive");

    std::vector<Vector<N>> vertices;
    std::vector<Line> lines;

    static Mesh makeCube() {
        Mesh m;

        const std::size_t vertexCount = std::size_t{1} << N;
        m.vertices.reserve(vertexCount);
        m.lines.reserve((vertexCount * N) / 2);

        for (std::size_t mask = 0; mask < vertexCount; ++mask) {
            Vector<N> vertex{};
            for (int dim = 0; dim < N; ++dim) {
                const std::size_t bit = std::size_t{1} << dim;
                vertex[dim] = (mask & bit) ? 1.0f : -1.0f;
            }
            m.vertices.push_back(vertex);
        }

        // Each edge connects two vertices that differ by exactly one bit.
        for (std::size_t mask = 0; mask < vertexCount; ++mask) {
            for (int dim = 0; dim < N; ++dim) {
                if (const std::size_t bit = std::size_t{1} << dim; (mask & bit) == 0) {
                    m.lines.push_back({
                        static_cast<int>(mask),
                        static_cast<int>(mask | bit)
                    });
                }
            }
        }

        return m;
    }

    static Mesh makeAxes(const float length = 2.0f) {
        Mesh m;

        Vector<N> origin{};
        Vector<N> up{};
        up[N - 1] = length;

        m.vertices = {origin, up};
        m.lines = {{0, 1}};

        return m;
    }
};

#endif
