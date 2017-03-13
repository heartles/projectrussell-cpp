#pragma once

#include <memory>

#include "../graphics.h"
#include "../math.h"

enum class ActionType
{
    None = 0,
    Move,
    Attack
};

struct Unit;
struct Mirage
{
    Sprite Spr = { 0 };
    ivec2 TilePos;

    Unit *Original;
};

struct Order
{
    ActionType Type = ActionType::None;
    Order *Next = nullptr;
    Mirage Mirage = { 0 };

    union
    {
        ivec2 TilePos;
        Unit *Other;
    };

    inline Order()
      : TilePos({}){};
};

struct Unit
{
    Sprite Spr = { 0 };
    ivec2 TilePos = {};

    // Linked list of orders to process
    Order *Orders = nullptr;
    int ID;

    Unit();
    Unit(Sprite spr, ivec2 pos);
    Unit(const Unit &) = default;

  private:
    static int _idCount;
};

struct Game;
void DrawUnit(Game *engine, const Unit &unit);
void DrawMirage(Game *engine, const Mirage &mirage);
vec2 GetTileCenter(const ivec2 &v);
