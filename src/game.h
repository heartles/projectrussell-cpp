#pragma once

#include <array>
#include <string>
#include <memory>

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

STRONG_TYPEDEF(ComponentID, uint64_t);

class GameComponent {
public:
	virtual ~GameComponent() {};
};

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
  return static_cast<int16_t>(x) > static_cast<int16_t>(y);
}

class Renderable
{
  RenderOrder _order;
protected:
  Renderable(RenderOrder x = RenderOrder(0))
    : _order(x) {}
public:
  virtual void Draw(Game &g) = 0;
  RenderOrder RequestedDrawOrder() const {
    return _order;
  }
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


};

void BeginTurnProcessing(Game &);

struct Input
{
    bool Keyboard[349];
    bool Mouse[8];
    float MouseX, MouseY;
};

struct Game
{
  template <typename T>
  using ref_arr = std::vector<std::reference_wrapper<T>>;

  float DT;
  Input OldInput;
  Input CurrentInput;

  vec4 ClearColor;

  OrthoView View;
  OrthoView Screen;

  ::Level Level;

  ContentManager Content;
	AudioManager Audio;

  std::vector<std::unique_ptr<GameComponent>> Components{};

  ref_arr<Renderable> Renderables;
  ref_arr<Updateable> Updateables;

  bool ShouldClose;

  template <typename T>
  void AddComponent(T &&v)
  {
    static_assert(std::is_base_of<GameComponent, T>::value,
                  "Unable to convert type T to GameComponent");

    auto val = std::make_unique<T>(std::move(v));
    
    // TODO: Postpone adds until after update
    if constexpr (std::is_base_of<Renderable, T>::value) {
      Renderables.push_back(*val);
      stdex::sort(Renderables,
                  [](const auto &a,
                      const auto &b) {
                      return a.get().RequestedDrawOrder() <
                              b.get().RequestedDrawOrder();
                  });
    }

    if constexpr (std::is_base_of<Updateable, T>::value) {
      Updateables.push_back(*val);
    }

    Components.push_back(std::unique_ptr<GameComponent>{ val.release() });
  }

  inline bool KeyDown(int id) const {
    return CurrentInput.Keyboard[id];
  }

    inline bool KeyPressed(int id) const
    {
        return CurrentInput.Keyboard[id] && !OldInput.Keyboard[id];
    }

    inline bool MousePressed(int id) const
    {
        return CurrentInput.Mouse[id] && !OldInput.Mouse[id];
    }

    inline Game(std::string dataDir)
      : Content(dataDir)
      , ShouldClose(false)
    {
    }

    template <typename T>
    T *GetRenderable()
    {
        for (auto &v : Renderables) {
            auto val = dynamic_cast<T *>(&v.get());
            if (val) return val;
        }

        return nullptr;
    }
};

void Game_Init(Game &);
void LoadLevel(const std::string &fileLoc, Game &info);
void ParseTileLayer(const Json::Value &layer, Level &info);
void Game_Update(Game &);
void Game_Render(Game &);
