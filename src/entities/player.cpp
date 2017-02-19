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
    bool right = Engine.KeyPressed('D');
    bool left = Engine.KeyPressed('A');
    bool up = Engine.KeyPressed('W');
    bool down = Engine.KeyPressed('S');

    vec2 vel = {};
    if (left)
        vel.x -= 1;
    if (right)
        vel.x += 1;
    if (up)
        vel.y += 1;
    if (down)
        vel.y -= 1;

    if (Engine.Input.Keyboard[GLFW_KEY_ESCAPE])
        Engine.ShouldClose = true;

    vec2 potentialPos = _pos + vel;

    Rectangle mask = { potentialPos.x, potentialPos.y, 0.5f, 0.5f };
    ResolveCollision(mask, &potentialPos, Engine);

    _pos = potentialPos;

    right = Engine.Input.Keyboard[GLFW_KEY_RIGHT];
    left = Engine.Input.Keyboard[GLFW_KEY_LEFT];
    up = Engine.Input.Keyboard[GLFW_KEY_UP];
    down = Engine.Input.Keyboard[GLFW_KEY_DOWN];

    vel = {};
    if (left)
        vel.x -= 1;
    if (right)
        vel.x += 1;
    if (up)
        vel.y += 1;
    if (down)
        vel.y -= 1;

    Engine.View.X += vel.x * Engine.DT;
    Engine.View.Y += vel.y * Engine.DT;

}

void
Player::Draw()
{
    Engine.View.DrawSpritePart(_spr, _pos, _sprPart, { .5f, .5f }, 0,
                               Colors::White);
}
