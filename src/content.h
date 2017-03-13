#pragma once

#include <cstdlib>
#include <map>
#include <string>

#include <glad/glad.h>

#include "graphics.h"
#include "shader.h"

std::string ReadAllText(const std::string& filename);

size_t GetFileSize(std::FILE* file);

struct FontDesc
{
    std::string Filename;
    int PxSize;
    const Shader* Shader;
};

struct FontDescCmp
{
    bool operator()(const FontDesc& v1, const FontDesc& v2) const
    {
        return (v1.Filename != v2.Filename)
                 ? v1.Filename < v2.Filename
                 : (v1.PxSize != v2.PxSize)
                     ? v1.PxSize < v2.PxSize
                     : (v1.Shader != v2.Shader) ? v1.Shader < v2.Shader : false;
    }
};

class ContentManager
{
    std::map<std::pair<std::string, std::string>, Shader> _shaders{};
    std::map<std::string, Texture> _textures{};
    std::map<FontDesc, Font, FontDescCmp> _fonts{};

    const Shader* _defaultFontShader;
    const std::string _dataDir;

  public:
      std::string ResolvePath(std::string path) const;

    ContentManager(const std::string &dataDir);

    const Shader* LoadShader(std::string vertPath, std::string fragPath);
    const Texture* LoadTexture(std::string filename);
    const Font* LoadFont(std::string filename, int pxSize,
                         const Shader* s = nullptr);
};
