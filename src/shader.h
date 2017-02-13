#pragma once

#include <string>

#include <glad/glad.h>

struct Shader
{
    GLuint _vertShader, _fragShader, _program;

    void Apply();
};

Shader DEBUG_LoadShader(std::string vertPath, std::string fragPath);
