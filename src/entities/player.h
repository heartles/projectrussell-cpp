#pragma once

#include <glm/glm.hpp>
#include <json/json-forwards.h>

#include "../entityCreator.h"
#include "../game.h"
#include "../graphics.h"
#include "../math.h"

class Player : public GameComponent
{
    Sprite _spr;
    vec2 _pos;
    Rectangle _sprPart;

  public:
    Player(Game& game, Level& level, const EntityDesc& json);

    virtual void Update() override;
    virtual void Draw() override;
};
