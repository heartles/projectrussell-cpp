#pragma once

#include <array>
#include <string>

#include <json/json.h>

#include "common.h"
#include "content.h"
#include "game/unit.h"
#include "graphics.h"
#include "math.h"
#include "pool.h"

struct Level;
struct Game;

class Updateable
{
public:
    virtual void Update(Game &g) = 0;
};

class Renderable
{
public:
    virtual void Draw(Game &g) = 0;
};

class GUIRenderable
{
public:
    virtual void DrawGUI(Game &g) = 0;
};

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

    Pool<Unit> Units;
    Pool<Order> Orders;
};

void ProcessTurn(Game &);

struct Game
{
    template<typename T>
    using refcount_arr = std::vector<std::shared_ptr<T>>;

    float DT;
    Input OldInput;
    struct Input Input;
    bool ShouldClose;

    vec4 ClearColor;

    OrthoView View;
    OrthoView Screen;

    Level Level;

    ContentManager Content;

    refcount_arr<GUIRenderable> GUIRenderables;
    refcount_arr<Renderable> Renderables;
    refcount_arr<Updateable> Updateables;

    template<typename T>
    void Add(T v)
    {
        static_assert(
            std::is_base_of<GUIRenderable, T>::value ||
            std::is_base_of<Renderable, T>::value ||
            std::is_base_of<Updateable, T>::value,
            "Unable to convert type T to any of the following: Controller, GUIRenderable, Renderable, Updateable"
            );

        auto val = make_shared<T>(std::move(v));

        // TODO: We know at compile time which interfaces T can convert to,
        // can we do the actual cast at compile time instead of runtime?
        if (std::is_base_of<GUIRenderable, T>::value) {
            GUIRenderables.push_back(std::dynamic_pointer_cast<GUIRenderable>(val));
        }

        if (std::is_base_of<Renderable, T>::value) {
            Renderables.push_back(std::dynamic_pointer_cast<Renderable>(val));
        }

        if (std::is_base_of<Updateable, T>::value) {
            Updateables.push_back(std::dynamic_pointer_cast<Updateable>(val));
        }
    }

    inline bool KeyPressed(int id) const
    {
        return Input.Keyboard[id] && !OldInput.Keyboard[id];
    }

    inline bool MousePressed(int id) const
    {
        return Input.Mouse[id] && !OldInput.Mouse[id];
    }

    inline Game(std::string dataDir)
      : Content(dataDir)
      , ShouldClose(false)
    {
    }
};

void Game_Init(Game&);
void LoadLevel(const std::string& fileLoc, Game& info);
void ParseTileLayer(const Json::Value& layer, Level& info);
void Game_Update(Game&);
void Game_Render(Game&);
