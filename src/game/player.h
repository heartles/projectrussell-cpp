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

class PlayerController : public Updateable, public Renderable
{
    Unit *_selected = nullptr;

    ActionButton *_selectedAction = nullptr;
    std::vector<ActionButton> _availableActions{ 8 };

    void selectUnit(Unit *u);
    void deselectUnit(Unit *u);

	// Sounds
	using Sound_t = const Sound *;
	Sound_t _snd_no;

  public:
	PlayerController(Game &);

    virtual void Update(Game &) override;
    virtual void Draw(Game &) override;
    virtual RenderOrder RequestedDrawOrder() override;
};
