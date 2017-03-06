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
struct Order
{
    ActionType Type = ActionType::None;
    Order* Next = nullptr;

    union
    {
        ivec2 TilePos;
        Unit* Other;
    };

    inline Order()
      : TilePos({}){};
};

struct Unit
{
    Sprite Spr;
    ivec2 TilePos;

    // Linked list of orders to process
    Order* Orders;
};

struct Game;
void DrawUnit(Game* engine, const Unit& unit);
vec2 GetTileCenter(const ivec2& v);
