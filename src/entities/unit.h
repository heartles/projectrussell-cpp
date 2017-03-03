#pragma once

#include "../graphics.h"
#include "../math.h"
struct Unit
{
    Sprite Spr;
    ivec2 TilePos;
};

struct Game;
void DrawUnit(Game* engine, const Unit& unit);
vec2 GetTileCenter(const ivec2& v);