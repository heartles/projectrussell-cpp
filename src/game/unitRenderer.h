#pragma once

#include "../game.h"
#include "unit.h"

class UnitRenderer
    : public Renderable
{
public:
    virtual void Draw(Game &) override;
};