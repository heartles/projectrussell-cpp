#pragma once

#include <cassert>
#include <cmath>

template <typename T>
inline T
sign(T val)
{
    return T((T(0) < val) - (val < T(0)));
}

template <typename T>
inline T
Max(T v1, T v2)
{
    if (v1 < v2)
        return v2;
    return v1;
}

template <typename T>
inline T
Min(T v1, T v2)
{
    if (v1 < v2)
        return v1;
    return v2;
}

template <typename T>
inline T
Clamp(T val, T min, T max)
{
    return Min(Max(val, min), max);
}

union vec2
{
    struct
    {
        float x, y;
    };
    float _arr[2];

    constexpr inline vec2(float xx, float yy)
      : x(xx)
      , y(yy)
    {
    }

    constexpr inline vec2()
      : x(0)
      , y(0)
    {
    }

    inline float& operator[](int index)
    {
        assert(index < 2);
        return _arr[index];
    }

    inline float operator[](int index) const
    {
        assert(index < 2);
        return _arr[index];
    }

    inline float Length() const { return std::sqrt(LengthSquared()); }

    inline float LengthSquared() const { return x * x + y * y; }

    inline vec2 Normalize() const
    {
        auto len = Length();

        if (len == 0)
            return vec2();
        return vec2{ x / len, y / len };
    }

    inline vec2 operator*(float scalar) const
    {
        return vec2{ x * scalar, y * scalar };
    }

    inline vec2 operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    inline vec2 operator+(vec2 other) const
    {
        return vec2{ x + other.x, y + other.y };
    }

    inline vec2 operator-(vec2 other) const
    {
        return vec2{ x - other.x, y - other.y };
    }

    inline vec2& operator+=(vec2 other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    inline vec2& operator-=(vec2 other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    inline vec2 operator/(float scalar) const
    {
        return { x / scalar, y / scalar };
    }

    static inline vec2 FromMagnitudeTheta(float magnitude, float theta)
    {
        return { magnitude * cos(theta), magnitude * sin(theta) };
    }
};

struct ivec2
{
    int x = 0, y = 0;

    constexpr inline ivec2 operator*(int scalar) const
    {
        return { x * scalar, y * scalar };
    }

    constexpr inline explicit ivec2(const vec2& v)
      : x(int(v.x))
      , y(int(v.y))
    {
    }

    constexpr inline ivec2(int xx, int yy)
      : x(xx)
      , y(yy)
    {
    }

    constexpr ivec2() = default;
    constexpr ivec2(const ivec2&) = default;
    ivec2(ivec2&&) = default;
    ivec2& operator=(const ivec2&) = default;
    ivec2& operator=(ivec2&&) = default;

    inline explicit operator vec2() const { return { float(x), float(y) }; }
};

constexpr bool
operator==(const ivec2& v1, const ivec2& v2)
{
    return (v1.x == v2.x) && (v1.y == v2.y);
}

struct vec3
{
    union
    {
        struct
        {
            float x, y, z;
        };
        struct
        {
            float r, g, b;
        };
        float _arr[3];
    };

    inline vec3(float xx, float yy, float zz)
    {
        x = xx;
        y = yy;
        z = zz;
    }

    inline vec3()
    {
        x = 0;
        y = 0;
    }

    inline float& operator[](int index)
    {
        assert(index < 3);
        return _arr[index];
    }

    inline float operator[](int index) const
    {
        assert(index < 3);
        return _arr[index];
    }

    inline float Length() const { return std::sqrt(LengthSquared()); }

    inline float LengthSquared() const { return x * x + y * y + z * z; }

    inline vec3 Normalize() const
    {
        auto len = Length();

        if (len == 0)
            return vec3();
        return vec3{ x / len, y / len, z / len };
    }
};

struct vec4
{
    union
    {
        struct
        {
            float x, y, z, w;
        };
        struct
        {
            float r, g, b, a;
        };
        float _arr[4];
    };

    inline float& operator[](int index)
    {
        assert(index < 2);
        return _arr[index];
    }
};

inline float 
Dot(vec2 v1, vec2 v2)
{
    float sum = 0;
    for (int i = 0; i < 2; i++) {
        sum += v1[i] + v2[i];
    }

    return sum;
}

inline float 
Dot(vec3 v1, vec3 v2)
{
    float sum = 0;
    for (int i = 0; i < 3; i++) {
        sum += v1[i] * v2[i];
    }

    return sum;
}

inline float 
Dot(vec4 v1, vec4 v2)
{
    float sum = 0;
    for (int i = 0; i < 4; i++) {
        sum += v1[i] + v2[i];
    }

    return sum;
}

inline vec3 
Cross(vec3 a, vec3 b)
{
    return {
        a[1] * b[2] - a[2] * b[1],
        a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0]
    };
}

inline bool
operator ==(const vec3 &a, const vec3& b)
{
    return a[0] == b[0] &&
        a[1] == b[1] &&
        a[2] == b[2];
}

inline bool
operator !=(const vec3 &a, const vec3& b)
{
    return !(a == b);
}


union mat2
{
    struct {
        float m00, m10,
            m01, m11;
    };
    vec2 _arr[2];

    inline mat2(vec2 col1, vec2 col2)
    {
        _arr[0] = col1;
        _arr[1] = col2;
    }

    inline vec2& operator [](int index)
    {
        assert(index < 2);
        return _arr[index];
    }

    inline const vec2 Row(int i) const
    {
        return vec2{
            _arr[0][i], _arr[1][i]
        };
    }

    inline float Det() const
    {
        return m00 * m11 - m01 * m10;
    }
};

// m(y,x) order
// column first
union mat3
{
    struct {
        float m00, m10, m20, 
            m01, m11, m21, 
            m02, m12, m22;
    };
    vec3 _arr[3];

    inline static mat3 FromCols(vec3 a, vec3 b, vec3 c)
    {
        mat3 m = { 0 };
        m._arr[0] = a;
        m._arr[1] = b;
        m._arr[2] = c;

        return m;
    }

    inline vec3& operator [](int index)
    {
        assert(index < 3);
        return _arr[index];
    }

    inline const vec3& operator [](int index) const
    {
        assert(index < 3);
        return _arr[index];
    }

    inline const vec3 Row(int i) const
    {
        return vec3{
            _arr[0][i], _arr[1][i], _arr[2][i]
        };
    }

    inline float Det() const
    {
        return m00 * mat2{ {m11, m21}, {m12, m22} }.Det() -
            m01 * mat2{ {m10, m20}, {m12, m22} }.Det() +
            m02 * mat2{ {m10, m20}, {m11, m21} }.Det();
    }

    static inline mat3 FromRows(vec3 r1, vec3 r2, vec3 r3)
    {
        return mat3{
            r1[0], r2[0], r3[0],
            r1[1], r2[1], r3[1],
            r1[2], r2[2], r3[2]
        };
    }

    inline mat3 Inverse() const
    {
        float det = Det();

        float a = m00, b = m01, c = m02, 
            d = m10, e = m11, f = m12, 
            g = m20, h = m21, i = m22;

        float A = e * i - f * h,
            B = -(d * i - f * g),
            C = d * h - e * g,
            D = -(b * i - c * h),
            E = a * i - c * g,
            F = -(a * h - b * g),
            G = b * f - c * e,
            H = -(a * f - c * d),
            I = a * e - b * d;

        return FromCols(
            { A / det, B / det, C / det },
            { D / det, E / det, F / det },
            { G / det, H / det, I / det });
    }
};

inline mat3
operator *(mat3 m1, mat3 m2)
{
    mat3 ret = { 0 };
    for (int col = 0; col < 3; col++) {
        for (int row = 0; row < 3; row++) {
            ret[col][row] = Dot(m1.Row(row), m2[col]);
        }
    }

    return ret;
}

inline vec3 
operator *(mat3 m, vec3 v)
{
    vec3 ret = {};

    for (int row = 0; row < 3; row++) {
        ret[row] = Dot(m.Row(row), v);
    }

    return ret;
}

inline bool
operator ==(const mat3 &a, const mat3 &b)
{
    return a[0] == b[0] &&
        a[1] == b[1] &&
        a[2] == b[2];
}

inline bool
operator !=(const mat3 &a, const mat3 &b)
{
    return !(a == b);
}


mat3 Translate(vec2 amount);

mat3 Scale(vec2 amount);

mat3 Rotate(float amount);

template <typename T>
inline constexpr T
Lerp(const T& v1, const T& v2, float amount)
{
    return (v1 + (v2 - v1) * amount);
}

inline int
Round(float f)
{
    return (fmod(f, 1.0f) < 0.5f) ? int(f) : int(f) + 1;
}

namespace Identity
{
    const mat3 Mat3 = mat3::FromCols(
        {1, 0, 0},
        {0, 1, 0},
        {0, 0, 1}
    );
}
