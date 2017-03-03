#include "unit.h"

#include "../game.h"

void
DrawUnit(Game* engine, const Unit& unit)
{
    engine->View.DrawSprite(unit.Spr, GetTileCenter(unit.TilePos), 0,
                            { .5f, .5f }, Colors::White);
}

vec2
GetTileCenter(const ivec2& v)
{
    return { float(v.x) + 0.5f, float(v.y) - 0.5f };
}
