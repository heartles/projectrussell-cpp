#pragma once

#include <array>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include <ft2build.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include FT_FREETYPE_H

#include "math.h"
#include "shader.h"

namespace Colors {
const vec4 Black = { 0, 0, 0, 1 };
const vec4 White = { 1, 1, 1, 1 };
const vec4 Red = { 1, 0, 0, 1 };
const vec4 Green = { 0, 1, 0, 1 };
const vec4 Blue = { 0, 0, 1, 1 };
};

struct Rectangle
{
    float X, Y, HalfWidth, HalfHeight;

    inline vec2 Min() const { return { X - HalfWidth, Y - HalfHeight }; }

    inline vec2 Max() const { return { X + HalfWidth, Y + HalfHeight }; }

    inline vec2 Pos() const { return { X, Y }; }

    inline float Width() const { return HalfWidth * 2; }
    inline float Height() const { return HalfHeight * 2; }

    inline vec2 Dim() const { return { HalfWidth * 2, HalfHeight * 2 }; }

    inline bool Intersects(const Rectangle& other) const
    {
        return Min().x < other.Max().x && Max().x > other.Min().x &&
               Min().y < other.Max().y && Max().y > other.Min().y;
    }

    inline bool Contains(const vec2& point) const
    {
        return point.x < Max().x && point.x > Min().x && point.y < Max().y &&
               point.y > Min().y;
    }

    static inline Rectangle FromCorner(vec2 mincorner, float w, float h)
    {
        Rectangle res{};
        res.HalfWidth = w / 2;
        res.HalfHeight = h / 2;
        res.X = mincorner.x + res.HalfWidth;
        res.Y = mincorner.y + res.HalfHeight;

        return res;
    }
};

const struct Rectangle FullImage = Rectangle::FromCorner({ 0.0f, 0.0f }, 1, 1);

struct Texture;
struct Sprite
{
    const Texture* Tex;
    Rectangle Part;
};

struct Texture
{
    int Width;
    int Height;
    std::vector<uint8_t> Memory;
    GLuint TextureID;

    inline Sprite Sprite() const
    {
        return { this, Rectangle{ 0.5f, 0.5f, 0.5f, 0.5f } };
    }
};

struct Character
{
    const Texture* Texture;
    ivec2 Offset;  // Offset from origin to left/top of char
    ivec2 Advance; // Offset to next char/line
};

class Font
{
    std::map<uint64_t, Character> _codepoints;
    const Shader* _shader;

  public:
    inline Font() {}

    inline Font(std::map<uint64_t, Character>&& p, const Shader* s)
      : _codepoints(p)
      , _shader(s)
    {
    }

    void RenderText(std::string, glm::mat3 matrix, vec4 color) const;
};

struct Game;
struct OrthoView : public Rectangle
{
    Game* Engine;

  public:
    Rectangle Viewport = { 0, 0, 1, 1 };

    inline glm::mat3 Matrix() const
    {
        return Translate({ Viewport.X, Viewport.Y }) *
               Scale({ Viewport.Width(), Viewport.Height() }) *
               Scale({ 1 / Width(), 1 / Height() }) * Translate({ -X, -Y });
    }

    inline OrthoView(const Rectangle& r, Game* e)
      : Engine(e)
    {
        *static_cast<Rectangle*>(this) = r;
    }

    inline OrthoView() = default;

    void DrawTexturePart(const Texture* tex, vec2 pos, Rectangle texPart,
                         vec2 scale, float rotation, vec4 color);

    inline void DrawTexturePart(const Texture* tex, vec2 pos, Rectangle texPart)
    {
        DrawTexturePart(tex, pos, texPart, { 1, 1 }, 0, Colors::White);
    }

    inline void DrawTexture(const Texture* tex, vec2 pos)
    {
        DrawTexturePart(tex, pos, FullImage);
    }

    inline void DrawTexture(const Texture* tex, vec2 pos, float rot)
    {
        DrawTexturePart(tex, pos, FullImage, { 1, 1 }, rot, Colors::White);
    }

    inline void DrawSprite(Sprite spr, vec2 pos, float rot, vec2 scale,
                           vec4 color)
    {
        DrawTexturePart(spr.Tex, pos, spr.Part, scale, rot, color);
    }

    void DrawRectangle(Rectangle coords, vec4 color);

    void RenderText(std::string text, const Font* f, vec2 pos, vec2 scale,
                    vec4 color);

    vec2 WorldToViewport(const vec2& val) const;
    vec2 ViewportToWorld(const vec2& val) const;
};

struct View
{
    Rectangle Game, Display;
};

struct Tileset
{
    std::string ImageName;
    int TileWidth, TileHeight;
    int TileCountX, TileCountY;
    int ImageWidth, ImageHeight;
    int TileCountTotal, FirstTileID;
    int Spacing, Margin;
    const Texture* Image;

    std::vector<vec2> Texcoords;
    std::vector<vec3> Positions;

    GLuint VertexBufferIDs[2];
    GLuint VertexArrayID;

    inline vec2 TopLeftFromGID(int gid)
    {
        return TopLeftFromID(gid - FirstTileID);
    }

    inline std::array<vec2, 4>&& CornerCoordsFromID(int id)
    {
        int tileX = id % TileCountX;
        int spacingOffsetX = Spacing * tileX;

        int minPixelX = tileX * TileWidth + spacingOffsetX;
        int maxPixelX = minPixelX + TileWidth;

        int tileY = id / TileCountX;
        int spacingOffsetY = Spacing * tileY;

        int minPixelY = tileY * TileHeight + spacingOffsetY;
        int maxPixelY = minPixelY + TileHeight;

        float minX = minPixelX / static_cast<float>(ImageWidth);
        float maxX = maxPixelX / static_cast<float>(ImageWidth);

        float minY = minPixelY / static_cast<float>(ImageHeight);
        float maxY = maxPixelY / static_cast<float>(ImageHeight);

        return std::move(std::array<vec2, 4>{
          { { minX, minY }, { maxX, minY }, { maxX, maxY }, { minX, maxY } } });
    }

    inline vec2 TopLeftFromID(int id)
    {
        if (id >= TileCountTotal) {
            throw std::out_of_range("ID out of range of tileset " + ImageName);
        }

        vec2 src = {
            static_cast<float>(id % TileCountX) * TileWidth / ImageWidth,
            static_cast<float>(id / TileCountX) * TileHeight / ImageHeight
        };

        src.x += static_cast<float>(id % TileCountX * Spacing) / ImageWidth;
        src.y += static_cast<float>((id / TileCountX) * Spacing) / ImageHeight;

        src.x += static_cast<float>(Margin) / ImageWidth;
        src.y += static_cast<float>(Margin) / ImageHeight;

        return src;
    }

    inline Rectangle SprPartFromID(int id)
    {
        vec2 tl = TopLeftFromID(id);

        return Rectangle::FromCorner(
          tl, static_cast<float>(TileWidth) / ImageWidth,
          static_cast<float>(TileHeight) / ImageHeight);
    }

    inline Sprite SpriteFromGID(int gid)
    {
        auto id = gid - FirstTileID;
        return Sprite{ Image, SprPartFromID(id) };
    }

    inline Rectangle SprPartFromGID(int gid)
    {
        auto id = gid - FirstTileID;
        vec2 tl = TopLeftFromID(id);

        return Rectangle::FromCorner(
          tl, static_cast<float>(TileWidth) / ImageWidth,
          static_cast<float>(TileHeight) / ImageHeight);
    }
};

Texture DEBUG_LoadTexture(std::string filename);

void SetUniform(std::string name, const glm::mat3& value);
void SetUniform(std::string name, const vec4& value);

void DEBUG_DrawTexture(const Texture* tex, glm::mat3 projection,
                       Rectangle texPart, vec4 color);

Font DEBUG_LoadFont(std::string filename, int pxSize, const Shader* s);