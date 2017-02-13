#include "player.h"

// TODO
#include <GLFW/glfw3.h>

Player::Player(Game& game, const vec2& pos)
  : GameComponent(game)
  , _pos(pos)
{

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
}

void
Player::Draw()
{
    Engine.View.DrawSpritePart(_spr, _pos, FullImage, { .5f, .5f }, 0,
                               Colors::White);
}
