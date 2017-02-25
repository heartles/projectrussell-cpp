#pragma once

#include <array>
#include <string>

#include <json/json.h>

#include "common.h"
#include "content.h"
#include "graphics.h"
#include "math.h"

typedef Sprite (*PFN_LOADIMAGE)(std::string filename);
typedef void (*PFN_DRAWSPRITE)(Sprite, struct Rectangle, struct Rectangle,
                               float);

#pragma pack(push)
struct Tileset
{
    std::string ImageName;
    int TileWidth, TileHeight;
    int TileCountX, TileCountY;
    int ImageWidth, ImageHeight;
    int TileCountTotal, FirstTileID;
    int Spacing, Margin;
    Sprite Image;

    std::vector<vec2> Texcoords;
    std::vector<vec3> Positions;

    GLuint VertexBufferIDs[2];
    GLuint VertexArrayID;

    inline vec2 TopLeftFromGID(int gid)
    {
        return TopLeftFromID(gid - FirstTileID);
    }

    inline std::array<vec2, 4> && CornerCoordsFromID(int id)
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
            { {
                    minX, minY
                },
                {
                    maxX, minY
                },
                {
                    maxX, maxY
                },
                {
                    minX, maxY
                }
            }});
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

    inline Rectangle SprPartFromGID(int gid)
    {
        auto id = gid - FirstTileID;
        vec2 tl = TopLeftFromID(id);

        return Rectangle::FromCorner(
          tl, static_cast<float>(TileWidth) / ImageWidth,
          static_cast<float>(TileHeight) / ImageHeight);
    }
};
#pragma pack(pop)

struct Level
{
    int Width, Height;
    std::vector<Tileset> Tilesets;
    int TileWidth, TileHeight;

    inline Tileset* GetTilesetFromGID(int gid)
    {
        for (auto& tileset : Tilesets) {
            if (gid - tileset.FirstTileID < tileset.TileCountTotal)
                return &tileset;
        }

        throw std::invalid_argument("tile gid is out of range of all tilesets");
    }
};

struct Game;
class GameComponent
{
  public:
    inline virtual void Update() {}
    inline virtual void Draw() {}
    inline virtual void DrawGUI() {}
    inline virtual ~GameComponent() {}
    Game& Engine;

  protected:
    inline GameComponent(Game& g)
      : Engine(g)
    {
    }
};

// Invariant: HalfWidth/HalfHeight are positive
struct BoundingBox
{
    Rectangle Rect;
};

struct Game
{
    std::string GameDir;

    float DT;
    Input OldInput;
    struct Input Input;
    bool ShouldClose;

    vec4 ClearColor;

    OrthoView View;
    OrthoView Screen;

    std::vector<GameComponent*> Components;
    Level Level;

    ContentManager Content;

    std::vector<BoundingBox> Statics;

    std::vector<GameComponent *> componentAddQueue, componentRmQueue;
    inline void AddComponent(GameComponent* c)
    {
        componentAddQueue.push_back(c);
    }
    inline void RemoveComponent(GameComponent* c)
    {
        componentRmQueue.push_back(c);
    }

    inline bool KeyPressed(int id)
    {
        return Input.Keyboard[id] && !OldInput.Keyboard[id];
    }

    inline bool MousePressed(int id)
    {
        return Input.Mouse[id] && !OldInput.Mouse[id];
    }

    template <typename T>
    inline T* GetInstanceOf()
    {
        for (auto c : Components) {
            T* val = dynamic_cast<T*>(c);
            if (val)
                return val;
        }

        return nullptr;
    }
};

void Game_Init(Game&);
void LoadLevel(const std::string& fileLoc, Game& info);
void ParseTileLayer(const Json::Value& layer, Level* info);
void Game_Update(Game&);
void Game_Render(Game&);
vec2 ToGame(Game& info, vec2 screen);
void ResolveCollision(Rectangle mask, vec2* pos, Game& engine);
