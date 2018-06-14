#pragma once

#include <cstdlib>
#include <map>
#include <string>

#include <glad/glad.h>

#include "graphics.h"
#include "shader.h"
#include "audio.h"

std::string ReadAllText(const std::string &filename);

struct FontDesc
{
    std::string Filename;
    int PxSize;
    const ::Shader *Shader;
};

struct FontDescCmp
{
    bool operator()(const FontDesc &v1, const FontDesc &v2) const
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
	std::map<std::string, Sound> _sounds{};

    const Shader *_defaultFontShader;
    const std::string _dataDir;

  public:
    std::string ResolvePath(const std::string &path) const;

    ContentManager(const std::string &dataDir);

    const Shader *LoadShader(const std::string &vertPath,
                             const std::string &fragPath);
    const Texture *LoadTexture(const std::string &filename);
    const Font *LoadFont(const std::string &filename,
                         int pxSize,
                         const Shader *s = nullptr);

	const Sound *LoadSound(const std::string &filename);
};
