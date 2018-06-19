#pragma once

#include <json/json-forwards.h>

#include "../entityCreator.h"
#include "../game.h"
#include "../graphics.h"
#include "../math.h"
#include "../pool.h"
#include "unit.h"

class GameEntity 
    : public GameComponent, public Updateable, public Renderable
{
  public:
    vec2 Pos;
    Sprite Spr;
    float Speed;
    
	GameEntity(Game &);

    virtual void Update(Game &) override;
    virtual void Draw(Game &) override;
};
