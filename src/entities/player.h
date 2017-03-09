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
    ActionType Type;
    std::string Name;
    Rectangle Box;
};

class PlayerController : public GameComponent
{
    Unit* _selected = nullptr;

    Pool<Order> _orders{};

    ActionButton* _selectedAction = nullptr;
    std::vector<ActionButton> _availableActions{ 8 };

    void selectUnit(Unit* u);
    void deselectUnit(Unit* u);

  public:
    inline PlayerController(Game& eng)
      : GameComponent(eng)
    {
    }

    void ProcessTurn();

    virtual void Update() override;
    virtual void DrawGUI() override;
};
