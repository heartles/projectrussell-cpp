#pragma once

#include <glm/glm.hpp>
#include <json/json-forwards.h>

#include "../entityCreator.h"
#include "../game.h"
#include "../graphics.h"
#include "../math.h"
#include "unit.h"

class PlayerController : public GameComponent
{
    Unit* _selected = nullptr;
    ActionType _selectedAction = ActionType::None;

  public:
    inline PlayerController(Game& eng)
      : GameComponent(eng)
    {
    }

    virtual void Update() override;
    virtual void DrawGUI() override;
};
