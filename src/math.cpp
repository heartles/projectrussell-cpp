#include "math.h"

#include "common.h"

mat3
Translate(vec2 amount)
{
    mat3 output = Identity::Mat3;
    vec3 &col3 = output[2];
    col3.x = amount.x;
    col3.y = amount.y;

    return output;
}

mat3
Scale(vec2 amount)
{
    mat3 output = Identity::Mat3;
    output[0][0] = amount.x;
    output[1][1] = amount.y;

    return output;
}

mat3
Rotate(float amount)
{
    mat3 output = Identity::Mat3;

    float cos = cosf(amount);
    float sin = sinf(amount);

    output[0][0] = cos;
    output[0][1] = -sin;
    output[1][0] = sin;
    output[1][1] = cos;

    return output;
}

int
AbsComponentSum(const ivec2 &v)
{
	return abs(v.x) + abs(v.y);
}

float
AbsComponentSum(const vec2 &v)
{
	return abs(v.x) + abs(v.y);
}

float
AbsComponentSum(const vec3 &v)
{
	return abs(v.x) + abs(v.y) + abs(v.z);
}
