#include "math.h"

glm::mat3
Translate(vec2 amount)
{
    glm::mat3 output = Identity<glm::mat3>();
    glm::vec3& col3 = output[2];
    col3.x = amount.x;
    col3.y = amount.y;

    return output;
}

glm::mat3
Scale(vec2 amount)
{
    glm::mat3 output = Identity<glm::mat3>();
    output[0][0] = amount.x;
    output[1][1] = amount.y;

    return output;
}

glm::mat3
Rotate(float amount)
{
    glm::mat3 output = Identity<glm::mat3>();

    float cos = cosf(amount);
    float sin = sinf(amount);

    output[0][0] = cos;
    output[0][1] = -sin;
    output[1][0] = sin;
    output[1][1] = cos;

    return output;
}
