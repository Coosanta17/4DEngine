#ifndef MATH_HPP
#define MATH_HPP

#include <cmath>
#include <type_traits>

template <int N>
struct VectorStorage {
    float data[N]{};
};

template <>
struct VectorStorage<3> {
    union {
        struct {
            float x;
            float y;
            float z;
        };

        float data[3];
    };
};

template <>
struct VectorStorage<4> {
    union {
        struct {
            float x;
            float y;
            float z;
            float w;
        };

        float data[4];
    };
};


template <int R, int C>
struct Matrix;

template <int N>
struct Vector : VectorStorage<N> {
    constexpr Vector() noexcept {
        for (int i = 0; i < N; ++i) this->data[i] = 0.0f;
    }

    template <typename... Args,
              typename = std::enable_if_t<sizeof...(Args) == N && (std::is_convertible_v<Args, float> && ...)>>
    constexpr explicit Vector(Args... values) noexcept {
        const float init[N]{static_cast<float>(values)...};
        for (int i = 0; i < N; ++i) this->data[i] = init[i];
    }

    Vector operator+(const Vector& other) const noexcept {
        Vector result{};
        for (int i = 0; i < N; ++i) result[i] = this->data[i] + other[i];
        return result;
    }

    Vector& operator+=(const Vector& other) noexcept {
        for (int i = 0; i < N; ++i) this->data[i] += other[i];
        return *this;
    }

    Vector operator-(const Vector& other) const noexcept {
        Vector result{};
        for (int i = 0; i < N; ++i) result[i] = this->data[i] - other[i];
        return result;
    }

    Vector operator-() const noexcept {
        Vector result{};
        for (int i = 0; i < N; ++i) result[i] = -this->data[i];
        return result;
    }

    Vector& operator-=(const Vector& other) noexcept {
        for (int i = 0; i < N; ++i) this->data[i] -= other[i];
        return *this;
    }

    Vector operator*(const double s) const noexcept {
        Vector result{};
        for (int i = 0; i < N; ++i) result[i] = this->data[i] * s;
        return result;
    }

    template <int R = N, int C>
    Vector<C> operator*(const Matrix<R, C>& m) noexcept {
        Vector<C> result{};
        for (int col = 0; col < C; ++col) {
            float sum = 0.0f;
            for (int row = 0; row < R; ++row)
                sum += this->data[row] * m[row][col];
            result[col] = sum;
        }
        return result;
    }

    [[nodiscard]] float dot(const Vector& other) const noexcept {
        float result = 0;
        for (int i = 0; i < N; ++i) result += this->data[i] * other[i];
        return result;
    }

    [[nodiscard]] float length() const noexcept {
        float sum = 0;
        for (int i = 0; i < N; ++i) sum += this->data[i] * this->data[i];
        return std::sqrt(sum);
    }

    [[nodiscard]] Vector normalise() const noexcept {
        const float l = this->length();
        Vector result{};
        if (l == 0.0f) return result;
        for (int i = 0; i < N; ++i) result[i] = this->data[i] / l;
        return result;
    }

    template <int M = N>
    [[nodiscard]] std::enable_if_t<M == 3, Vector> cross(const Vector& other) const noexcept {
        return Vector{
            this->data[1] * other[2] - this->data[2] * other[1],
            this->data[2] * other[0] - this->data[0] * other[2],
            this->data[0] * other[1] - this->data[1] * other[0]
        };
    }

    // Access by index (vec[i])
    float& operator[](int i) noexcept { return this->data[i]; }
    const float& operator[](int i) const noexcept { return this->data[i]; }
};

// Free function to support scalar * vector (this is a fucking mess)
template <int N>
Vector<N> operator*(const double s, const Vector<N>& v) noexcept {
    return v * s;
}

template <int R, int C>
struct Matrix {
    static_assert(R > 0 && C > 0, "Matrix dimensions must be positive");

    using Row = Vector<C>;

    float data[R][C]{};

    constexpr Matrix() noexcept {
        for (int row = 0; row < R; ++row)
            for (int col = 0; col < C; ++col)
                data[row][col] = 0.0f;
    }

    template <typename... Args,
              typename = std::enable_if_t<sizeof...(Args) == R * C && (std::is_convertible_v<Args, float> && ...)>>
    constexpr explicit Matrix(Args... values) noexcept {
        const float init[R * C]{static_cast<float>(values)...};
        for (int i = 0; i < R * C; ++i)
            data[i / C][i % C] = init[i];
    }

    template <typename... Rows,
              typename = std::enable_if_t<sizeof...(Rows) == R && (std::is_same_v<std::remove_cvref_t<Rows>, Row> && ...
              )>>
    constexpr explicit Matrix(const Rows&... rows) noexcept {
        const Row init[R]{rows...};
        for (int row = 0; row < R; ++row)
            for (int col = 0; col < C; ++col)
                data[row][col] = init[row][col];
    }

    template <int RR = R, int CC = C>
    [[nodiscard]] static constexpr std::enable_if_t<RR == CC, Matrix> identity() noexcept {
        Matrix result{};
        for (int i = 0; i < RR; ++i) result[i][i] = 1.0f;
        return result;
    }

    Matrix operator+(const Matrix& other) const noexcept {
        Matrix result{};
        for (int row = 0; row < R; ++row)
            for (int col = 0; col < C; ++col)
                result[row][col] = data[row][col] + other[row][col];
        return result;
    }

    Matrix& operator+=(const Matrix& other) noexcept {
        for (int row = 0; row < R; ++row)
            for (int col = 0; col < C; ++col)
                data[row][col] += other[row][col];
        return *this;
    }

    Matrix operator-(const Matrix& other) const noexcept {
        Matrix result{};
        for (int row = 0; row < R; ++row)
            for (int col = 0; col < C; ++col)
                result[row][col] = data[row][col] - other[row][col];
        return result;
    }

    Matrix operator-() const noexcept {
        Matrix result{};
        for (int row = 0; row < R; ++row)
            for (int col = 0; col < C; ++col)
                result[row][col] = -data[row][col];
        return result;
    }

    Matrix& operator-=(const Matrix& other) noexcept {
        for (int row = 0; row < R; ++row)
            for (int col = 0; col < C; ++col)
                data[row][col] -= other[row][col];
        return *this;
    }

    Matrix operator*(const double s) const noexcept {
        Matrix result{};
        for (int row = 0; row < R; ++row)
            for (int col = 0; col < C; ++col)
                result[row][col] = data[row][col] * s;
        return result;
    }

    Matrix& operator*=(const double s) noexcept {
        for (int row = 0; row < R; ++row)
            for (int col = 0; col < C; ++col)
                data[row][col] *= s;
        return *this;
    }

    Matrix operator/(const double s) const noexcept {
        Matrix result{};
        for (int row = 0; row < R; ++row)
            for (int col = 0; col < C; ++col)
                result[row][col] = data[row][col] / s;
        return result;
    }

    Matrix& operator/=(const double s) noexcept {
        for (int row = 0; row < R; ++row)
            for (int col = 0; col < C; ++col)
                data[row][col] /= s;
        return *this;
    }

    [[nodiscard]] bool operator==(const Matrix& other) const noexcept {
        for (int row = 0; row < R; ++row)
            for (int col = 0; col < C; ++col)
                if (data[row][col] != other[row][col]) return false;
        return true;
    }

    [[nodiscard]] bool operator!=(const Matrix& other) const noexcept {
        return !(*this == other);
    }

    template <int K>
    [[nodiscard]] Matrix<R, K> operator*(const Matrix<C, K>& other) const noexcept {
        Matrix<R, K> result{};
        for (int row = 0; row < R; ++row) {
            for (int k = 0; k < K; ++k) {
                float sum = 0.0f;
                for (int col = 0; col < C; ++col)
                    sum += data[row][col] * other[col][k];
                result[row][k] = sum;
            }
        }
        return result;
    }

    [[nodiscard]] Vector<R> operator*(const Vector<C>& v) const noexcept {
        Vector<R> result{};
        for (int row = 0; row < R; ++row) {
            float sum = 0.0f;
            for (int col = 0; col < C; ++col)
                sum += data[row][col] * v[col];
            result[row] = sum;
        }
        return result;
    }

    [[nodiscard]] Matrix<C, R> transpose() const noexcept {
        Matrix<C, R> result{};
        for (int row = 0; row < R; ++row)
            for (int col = 0; col < C; ++col)
                result[col][row] = data[row][col];
        return result;
    }

    float* operator[](int row) noexcept { return data[row]; }
    const float* operator[](int row) const noexcept { return data[row]; }

    float& operator()(int row, int col) noexcept { return data[row][col]; }
    const float& operator()(int row, int col) const noexcept { return data[row][col]; }
};

template <int R, int C>
Matrix<R, C> operator*(const double s, const Matrix<R, C>& m) noexcept {
    return m * s;
}

template <int R, int C>
using Mat = Matrix<R, C>;

using Vec3 = Vector<3>;
using Vec4 = Vector<4>;

using Mat2 = Matrix<2, 2>;
using Mat3 = Matrix<3, 3>;
using Mat4 = Matrix<4, 4>;
using Mat3x4 = Matrix<3, 4>;
using Mat4x3 = Matrix<4, 3>;

[[nodiscard]] inline Vec3 cross(const Vec3& a, const Vec3& b) noexcept {
    return a.cross(b);
}

// TODO Make cleaner to suit within the current structure in a more maintainable fashion.
// FIXME These matrices aren't even being used
inline Mat4 rotY(const float a){
    Mat4 r = Mat4::identity();
    r.data[0][0]=cos(a);
    r.data[0][2]=sin(a);
    r.data[2][0]=-sin(a);
    r.data[2][2]=cos(a);
    return r;
}

inline Mat4 rotX(const float a){
    Mat4 r = Mat4::identity();
    r.data[1][1]=cos(a);
    r.data[1][2]=-sin(a);
    r.data[2][1]=sin(a);
    r.data[2][2]=cos(a);
    return r;
}

#endif
