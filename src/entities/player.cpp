#include "player.h"

#include <json/json.h>

// TODO
#include <GLFW/glfw3.h>

Player::Player(Game& game, Level& level, const EntityDesc& obj)
  : GameComponent(game)
{
    _spr =
      Engine.Content.LoadSprite(Engine.GameDir + "/content/roguelikeChar.png");

    _pos = obj.Pos; 
    _moveTarget = _pos;

    _sprPart = level.GetTilesetFromGID(obj.TileGID)->SprPartFromGID(obj.TileGID);
}

ivec2 GetTilePos(Game &engine, vec2 screen)
{
    auto game = ToGame(engine, screen); //TODO: should this be inlined?
    return { int(game.x), int(game.y) };
}

vec2 TilePosToFloat(ivec2 tile)
{
    return { tile.x + 0.5f, tile.y - 0.5f };
}

void
Player::Update()
{
    if (Engine.Input.Keyboard[GLFW_KEY_ESCAPE])
        Engine.ShouldClose = true;

    if (Engine.MousePressed(0)) {
        _moveTarget = TilePosToFloat(GetTilePos(Engine, { Engine.Input.MouseX, Engine.Input.MouseY }));
    }

    vec2 vel = {};
    vel = _moveTarget - _pos;
    vel = vel.Normalize();
    vel *= Min((_moveTarget - _pos).Length(), Engine.DT * 3);


    vec2 potentialPos = _pos + vel;

    Rectangle mask = { potentialPos.x, potentialPos.y, 0.5f, 0.5f };
    ResolveCollision(mask, &potentialPos, Engine);

    _pos = potentialPos;
}

void
Player::Draw()
{
    Engine.View.DrawSpritePart(_spr, _pos, _sprPart, { .5f, .5f }, 0,
                               Colors::White);
}
