#include "shader.h"
#include "common.h"
#include "content.h"
#include <cstddef>
#include <vector>

void
Shader::Apply() const
{
    glUseProgram(_program);
}

Shader
DEBUG_LoadShader(std::string vertPath, std::string fragPath)
{
    auto v = glCreateShader(GL_VERTEX_SHADER);
    auto f = glCreateShader(GL_FRAGMENT_SHADER);
    Shader s{ v, f };

    auto vert = ReadAllText(vertPath);
    char const* vertPtr = vert.c_str();
    glShaderSource(s._vertShader, 1, &vertPtr, nullptr);

    glCompileShader(s._vertShader);
    GLint success;
    glGetShaderiv(s._vertShader, GL_COMPILE_STATUS, (GLint*)&success);
    if (!success) {
        GLint maxLength;
        glGetShaderiv(s._vertShader, GL_INFO_LOG_LENGTH, &maxLength);
        if (maxLength) {
            std::vector<GLchar> infoLog((int)maxLength);
            glGetShaderInfoLog(s._vertShader, maxLength, &maxLength,
                               infoLog.data());

            Log((char*)infoLog.data());
        }
    }

    auto frag = ReadAllText(fragPath);
    char const* fragPtr = frag.c_str();
    glShaderSource(s._fragShader, 1, &fragPtr, nullptr);

    glCompileShader(s._fragShader);
    glGetShaderiv(s._fragShader, GL_COMPILE_STATUS, (GLint*)&success);
    if (!success) {
        GLint maxLength;
        glGetShaderiv(s._fragShader, GL_INFO_LOG_LENGTH, &maxLength);
        if (maxLength) {
            std::vector<GLchar> infoLog((int)maxLength);
            glGetShaderInfoLog(s._fragShader, maxLength, &maxLength,
                               infoLog.data());

            Log((char*)infoLog.data());
        }
    }

    s._program = glCreateProgram();

    glAttachShader(s._program, s._vertShader);
    glAttachShader(s._program, s._fragShader);
    glLinkProgram(s._program);

    GLint isLinked;
    glGetProgramiv(s._program, GL_LINK_STATUS, (int*)&isLinked);
    if (!isLinked) {
        GLint maxLength;
        glGetProgramiv(s._program, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog((int)maxLength);
        glGetProgramInfoLog(s._program, maxLength, &maxLength, infoLog.data());

        Log((char*)infoLog.data());
    }

    return s;
}
