#pragma once

#include <glm/glm.hpp>

#include "../game.h"
#include "../graphics.h"
#include "../math.h"

class Player : public GameComponent
{
    Sprite _spr;
    vec2 _pos;
 
  public:

    Player(Game& game, const vec2& pos);

    virtual void Update() override;
    virtual void Draw() override;
};
