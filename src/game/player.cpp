#include "player.h"

#include <json/json.h>

#include "../mathstrings.h"

// TODO
#include <GLFW/glfw3.h>

GameEntity::GameEntity(Game &g)
{
}

void
GameEntity::Update(Game &Engine)
{
  vec2 velocity{};
  if (Engine.KeyDown('W'))
    velocity.y += 1;
  if (Engine.KeyDown('A'))
    velocity.x -= 1;
  if (Engine.KeyDown('S'))
    velocity.y -= 1;
  if (Engine.KeyDown('D'))
    velocity.x += 1;

  velocity = velocity.Normalize();

  Pos += velocity * Speed;
}

void
GameEntity::Draw(Game &Engine)
{
  Engine.View.DrawSprite(Spr, Pos, 0, vec2{1, 1}, Colors::White);
}
