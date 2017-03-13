#include "unit.h"

#include "../game.h"

int Unit::_idCount;

void
DrawUnit(Game* engine, const Unit& unit)
{
    engine->View.DrawSprite(unit.Spr, GetTileCenter(unit.TilePos), 0,
                            { .5f, .5f }, Colors::White);
}

void
DrawMirage(Game* engine, const Mirage& m)
{
    engine->View.DrawSprite(m.Spr, GetTileCenter(m.TilePos), 0, { 0.5f, 0.5f }, vec4{ 0.8f, 0.8f, 1, 0.7f });
}

vec2
GetTileCenter(const ivec2& v)
{
    return { static_cast<float>(v.x) + 0.5f, static_cast<float>(v.y) - 0.5f };
}

Unit::Unit()
{
    ID = _idCount++;
}

Unit::Unit(Sprite spr, ivec2 pos)
{
    Spr = spr;
    TilePos = pos;
    ID = _idCount++;
}
