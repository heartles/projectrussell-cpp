#pragma once

#include <json/json-forwards.h>

#include "../entityCreator.h"
#include "../game.h"
#include "../graphics.h"
#include "../math.h"
#include "../pool.h"
#include "unit.h"

struct ActionButton
{
    ActionType Type = ActionType::None;
    std::string Name{};
    Rectangle Box = { 0 };
};

class PlayerController
    : public Updateable, public GUIRenderable
{
    Unit* _selected = nullptr;

    ActionButton* _selectedAction = nullptr;
    std::vector<ActionButton> _availableActions{ 8 };

    void selectUnit(Unit* u);
    void deselectUnit(Unit* u);

  public:
      virtual void Update(Game &) override;
      virtual void DrawGUI(Game &) override;
};
