#include "unit.h"

#include "../game.h"

// UnitID Unit::s_idCount = 1;

vec2
GetTileCenter(const ivec2 &v)
{
    return { static_cast<float>(v.x) + 0.5f, static_cast<float>(v.y) - 0.5f };
}
#if 0
Unit::Unit()
{
    _id = s_idCount++;
	StdActionRemaining = 6;
	MoveActionRemaining = 6;
}

Unit::Unit(Sprite spr, ivec2 pos)
{
    Spr = spr;
    TilePos = pos;
    _id = s_idCount++;
}
#endif