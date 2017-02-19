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

    _sprPart = level.GetTilesetFromGID(obj.TileGID)->SprPartFromGID(obj.TileGID);
}

void
Player::Update()
{
    if (Engine.Input.Keyboard[GLFW_KEY_ESCAPE])
        Engine.ShouldClose = true;

    if (Engine.MousePressed(0)) {
        _moveTarget = ToGame(Engine, { Engine.Input.MouseX, Engine.Input.MouseY });
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
