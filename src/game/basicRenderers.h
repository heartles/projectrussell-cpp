#pragma once

#include "../game.h"
#include "unit.h"

class TilemapRenderer : public GameComponent, public Renderable
{
  public:

    TilemapRenderer()
      : Renderable(RenderOrder(100)) {}
    virtual void Draw(Game &) override;
};