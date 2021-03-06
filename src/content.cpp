#include "content.h"

#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>

#include "common.h"
#include "game.h"

#include <glad/glad.h>
#include <stb/image.h>
#include <audiodecoder.h>

std::string
ReadAllText(const std::string &filename)
{
    std::stringstream buf{};

    std::ifstream file{ filename };
    buf << file.rdbuf();

    return buf.str();
}

ContentManager::ContentManager(const std::string &dataDir)
  : _dataDir(dataDir)
{
    _defaultFontShader =
      LoadShader("/content/text.gl.vert", "/content/text.gl.frag");
}

std::string
ContentManager::ResolvePath(const std::string &path) const
{
    return _dataDir + path;
}

const Shader *
ContentManager::LoadShader(const std::string &vp, const std::string &fp)
{
    std::string vertPath = _dataDir + vp, fragPath = _dataDir + fp;
    if (_shaders.count({ vertPath, fragPath })) {
        return &_shaders[{ vertPath, fragPath }];
    }

    auto s = DEBUG_LoadShader(vertPath, fragPath);
    _shaders[{ vertPath, fragPath }] = s;

    return &_shaders[{ vertPath, fragPath }];
}

const Texture *
ContentManager::LoadTexture(const std::string &f)
{
    std::string filename = _dataDir + f;

    if (_textures.count(filename)) {
        return &_textures[filename];
    }

    auto s = DEBUG_LoadTexture(filename);
    _textures[filename] = s;
    return &_textures[filename];
}

const Font *
ContentManager::LoadFont(const std::string &f, int pxSize, const Shader *shader)
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

const Sound *
ContentManager::LoadSound(const std::string &f)
{
	std::string filename = _dataDir + f;

    SF_INFO info{};
    SNDFILE *file = sf_open(filename.c_str(), SFM_READ, &info);
	assert(file); // TODO: Graceful error handling

	int samplesToRead = info.frames;

	std::vector<float> samples(samplesToRead);

    sf_read_float(file, samples.data(), samples.size());
	
	Sound s = { std::move(samples), info.samplerate, info.channels };
	_sounds[filename] = s;

    sf_close(file);
	return &_sounds[filename];
}