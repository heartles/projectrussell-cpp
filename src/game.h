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
#include "audio.h"

struct Level;
struct Game;

class Updateable
{
  public:
    virtual void Update(Game &g) = 0;
};

class RenderOrder
{
    int16_t _value;

  public:
    static constexpr int16_t GUI_FLAG = 1 << 15;

    // Convention: negative values are game layers, positive values are GUI
    // The layer value cannot equal 1 << 15, ie INT16_MIN
    inline explicit RenderOrder(int16_t v)
    {
        // precondition: v != 0b1000_0000_0000_0000
        assert(v != GUI_FLAG);
        _value = v;
    }

    inline explicit operator int16_t() const { return _value; }
};

inline bool
operator<(const RenderOrder &x, const RenderOrder &y)
{
    return static_cast<int16_t>(x) < static_cast<int16_t>(y);
}

class Renderable
{
  public:
    virtual void Draw(Game &g) = 0;
    virtual RenderOrder RequestedDrawOrder() = 0;
};

struct Level
{
    int Width, Height;
    std::vector<Tileset> Tilesets;
    int TileWidth, TileHeight;

    inline Tileset *GetTilesetFromGID(int gid)
    {
        for (auto &tileset : Tilesets) {
            if (gid - tileset.FirstTileID < tileset.TileCountTotal)
                return &tileset;
        }

        throw std::invalid_argument("tile gid is out of range of all tilesets");
    }

    Pool<Unit> Units;
    Pool<Order> Orders;
};

void ProcessTurn(Game &);

struct Input
{
    bool Keyboard[349];
    bool Mouse[8];
    float MouseX, MouseY;
};

struct Game
{
    template <typename T>
    using refcount_arr = std::vector<std::shared_ptr<T>>;

    float DT;
    Input OldInput;
    Input Input;
    bool ShouldClose;

    vec4 ClearColor;

    OrthoView View;
    OrthoView Screen;

    Level Level;

    ContentManager Content;
	AudioManager Audio;

    refcount_arr<Renderable> Renderables;
    refcount_arr<Updateable> Updateables;

    template <typename T>
    void Add(T v)
    {
        static_assert(std::is_base_of<Renderable, T>::value ||
                        std::is_base_of<Updateable, T>::value,
                      "Unable to convert type T to any of the following: "
                      "Renderable, Updateable");

        auto val = make_shared<T>(std::move(v));

        // TODO: We know at compile time which interfaces T can convert to,
        // can we do the actual cast at compile time instead of runtime?
        if (std::is_base_of<Renderable, T>::value) {
            Renderables.push_back(std::dynamic_pointer_cast<Renderable>(val));
            stdex::sort(Renderables,
                        [](const std::shared_ptr<Renderable> &a,
                           const std::shared_ptr<Renderable> &b) {
                            return a->RequestedDrawOrder() <
                                   b->RequestedDrawOrder();
                        });
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

void Game_Init(Game &);
void LoadLevel(const std::string &fileLoc, Game &info);
void ParseTileLayer(const Json::Value &layer, Level &info);
void Game_Update(Game &);
void Game_Render(Game &);
