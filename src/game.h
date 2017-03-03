#pragma once

#include <array>
#include <string>

#include <json/json.h>

#include "common.h"
#include "content.h"
#include "entities/unit.h"
#include "graphics.h"
#include "math.h"

typedef Texture (*PFN_LOADIMAGE)(std::string filename);
typedef void (*PFN_DRAWSPRITE)(Texture, struct Rectangle, struct Rectangle,
                               float);

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

    std::vector<Unit> Units;

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
