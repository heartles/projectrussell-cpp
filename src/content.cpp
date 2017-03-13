#define _CRT_SECURE_NO_WARNINGS
#include "content.h"
#include "common.h"
#include "game.h"
#include <cstdio>
#include <cstdlib>
#include <glad/glad.h>
#include <stb/image.h>
#include <string>

size_t
GetFileSize(std::FILE* file)
{
    auto save = std::ftell(file);

    std::fseek(file, 0, SEEK_END);
    auto ret = std::ftell(file);

    std::fseek(file, save, SEEK_SET);

    return ret;
}

std::string
ReadAllText(const std::string& filename)
{
    Log(filename);
    auto file = std::fopen(filename.c_str(), "rb");
    if (!file) {
        // TODO
        Log("error reading file " + filename);
        std::exit(-1);
    }

    size_t fileSize = GetFileSize(file);
    auto buf = new char[fileSize + 1];

    auto res = std::fread(buf, 1, fileSize, file);
    if (res != fileSize) {
        Log("error reading file " + filename);
        std::exit(-1);
    }

    // Add our null terminated byte
    buf[fileSize] = 0;

    std::fclose(file);
    std::string str = buf;
    delete[] buf;
    return str;
}

ContentManager::ContentManager(const std::string &dataDir)
  : _dataDir(dataDir)
{
    _defaultFontShader =
      LoadShader("/content/text.gl.vert", "/content/text.gl.frag");
}

std::string ContentManager::ResolvePath(std::string path) const
{
    return _dataDir + path;
}

const Shader*
ContentManager::LoadShader(std::string vp, std::string fp)
{
    std::string vertPath = _dataDir + vp, fragPath = _dataDir + fp;
    if (_shaders.count({ vertPath, fragPath })) {
        return &_shaders[{ vertPath, fragPath }];
    }

    auto s = DEBUG_LoadShader(vertPath, fragPath);
    _shaders[{ vertPath, fragPath }] = s;

    return &_shaders[{ vertPath, fragPath }];
}

const Texture*
ContentManager::LoadTexture(std::string f)
{
    std::string filename = _dataDir + f;

    if (_textures.count(filename)) {
        return &_textures[filename];
    }

    auto s = DEBUG_LoadTexture(filename);
    _textures[filename] = s;
    return &_textures[filename];
}

const Font*
ContentManager::LoadFont(std::string f, int pxSize, const Shader* shader)
{
    if (shader == nullptr) {
        shader = _defaultFontShader;
    }

    std::string filename = f; // TODO: better filesystem abstraction for fonts
    FontDesc d{ filename, pxSize, shader };

    if (_fonts.count(d)) {
        return &_fonts[d];
    }

    auto font = DEBUG_LoadFont(filename, pxSize, shader);
    _fonts[d] = font;
    return &_fonts[d];
}