#pragma once

#include <cstdlib>
#include <map>
#include <string>

#include <glad/glad.h>

#include "graphics.h"
#include "shader.h"

std::string ReadAllText(const std::string filename);

size_t GetFileSize(std::FILE* file);

class ContentManager
{
    std::map<std::pair<std::string, std::string>, Shader> _shaders{};
    std::map<std::string, Texture> _textures{};

  public:
    Shader& LoadShader(std::string vertPath, std::string fragPath);
    const Texture* LoadTexture(std::string filename);
};
