#pragma once

#include "../game.h"
#include "unit.h"

class UnitRenderer : public Renderable
{
	UnitID _unitRendering = Unit::NullID;
	float _animProgress;
  public:
    virtual void Draw(Game &) override;
    virtual RenderOrder RequestedDrawOrder() override;
	void BeginRenderAnimations(Game &);
};

class TilemapRenderer : public Renderable
{
  public:
    virtual void Draw(Game &) override;
    virtual RenderOrder RequestedDrawOrder() override;
};