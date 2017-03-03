#pragma once

#include <cassert>
#include <glm/glm.hpp>

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
        : x(xx), y(yy)
    { }

    constexpr inline vec2()
        : x(0), y(0)
    { }

    inline float& operator[](int index)
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

    constexpr inline explicit ivec2(const vec2 &v)
        : x(int(v.x)), y(int(v.y)) {}

    constexpr inline ivec2(int xx, int yy) 
        : x(xx), y(yy)
    {}

    ivec2() = default;
    ivec2(const ivec2&) = default;
    ivec2(ivec2&&) = default;
    ivec2& operator=(const ivec2&) = default;
    ivec2& operator=(ivec2&&) = default;


    inline explicit operator vec2() const { return { float(x), float(y) }; }
};

constexpr bool operator==(const ivec2 &v1, const ivec2 &v2)
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
};

template <typename T>
inline T
normalize_zero(const T& vector)
{
    if (glm::length(vector) == 0)
        return T();

    return glm::normalize(vector);
}

template <typename T>
inline T
Identity()
{
    T output = {};

    for (int i = 0; i < output.length(); ++i) {
        output[i][i] = 1;
    }

    return output;
}

glm::mat3 Translate(vec2 amount);

glm::mat3 Scale(vec2 amount);

glm::mat3 Rotate(float amount);

template <typename T>
inline constexpr T
Lerp(const T& v1, const T& v2, float amount)
{
    return (v1 + (v2 - v1) * amount);
}

/*
inline
template<typename T>
T
sign(T val) {
    return T(T(0) < val) - (val < T(0));
}
*/
