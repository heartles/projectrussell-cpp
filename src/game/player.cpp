#include "player.h"

#include <json/json.h>

#include "../mathstrings.h"

// TODO
#include <GLFW/glfw3.h>

PlayerController::PlayerController(Game &g)
{
	_snd_no = g.Content.LoadSound("/content/no.wav");
	_availableActions.reserve(8);
}

ivec2
GetTilePos(const Game &engine, vec2 screen)
{
    auto game = engine.View.ViewportToWorld(screen);
    return { static_cast<int>(game.x), static_cast<int>(game.y) };
}

vec2
TilePosToFloat(ivec2 tile)
{
    return { tile.x + 0.5f, tile.y - 0.5f };
}

void
PlayerController::selectUnit(Unit *u)
{
    _selected = u;
    _selectedAction = nullptr;
	float buttonX = 60;
	for (auto &a : u->PossibleActions) {
		_availableActions.push_back({ &a,
			Rectangle::FromCorner({ buttonX, 60 }, 130, 60)
		});
		buttonX += 140;
	}
}

void
PlayerController::deselectUnit(Unit *u)
{
    _selected = nullptr;
    _selectedAction = nullptr;
    _availableActions.clear();
}

Order *
GetLastOrder(Unit *u)
{
	Order **v = &u->Orders;
	Order *prevOrder = nullptr;
	while (*v) {
		prevOrder = *v;
		v = &(*v)->Next;
	}

	return prevOrder;
}

Mirage *
GetLastMirage(Unit *u)
{
	auto o = GetLastOrder(u);
	return (o == nullptr) ? static_cast<Mirage *>(u) : &o->Mirage;
}

void
EnqueueOrder(Unit *u, Order *o)
{
	auto last = GetLastOrder(u);
	if (last == nullptr)
		u->Orders = o;
	else {
		last->Next = o;
		o->Last = last;
	}
}

void
PlayerController::Update(Game &Engine)
{
    const bool m = Engine.MousePressed(0);
    const auto mousePos =
      GetTilePos(Engine, { Engine.Input.MouseX, Engine.Input.MouseY });
    if (Engine.MousePressed(0)) {
        bool clickedOnUnit = false;
        if (!_selectedAction || std::get_if<NullAction>(_selectedAction->Action)) {
            for (auto &u : Engine.Level.Units) {
                if (u.TilePos == mousePos) {
                    if (&u != _selected) {
                        selectUnit(&u);
                    } else {
                        deselectUnit(_selected);
                    }

                    clickedOnUnit = true;
                }
            }
        }

        bool clickedOnButton = false;
        const vec2 screenMousePos = Engine.Screen.ViewportToWorld(
          { Engine.Input.MouseX, Engine.Input.MouseY });
        if (_selected) {
            for (auto &button : _availableActions) {
                if (button.Box.Contains(screenMousePos)) {
                    if (_selectedAction == &button) {
                        _selectedAction = nullptr;
                    } else {
                        _selectedAction = &button;
                    }

                    clickedOnButton = true;
                }
            }
        }

        if (!clickedOnButton && !clickedOnUnit) {
            const auto endTurnButton =
              Rectangle::FromCorner({ 1920 - 180, 60 }, 150, 60);
            if (endTurnButton.Contains(screenMousePos)) {
                BeginTurnProcessing(Engine);
                return;
            }

            if (_selectedAction) {
				std::visit(make_overload(
					[](NullAction) {},
					[&](const MoveAction &action)
					{
						const auto m = GetLastMirage(_selected);
						int tileDiff = std::abs(mousePos.x - m->TilePos.x) +
							std::abs(mousePos.y - m->TilePos.y);

						if (tileDiff > m->MoveActionRemaining + m->StdActionRemaining) {
							Engine.Audio.PlaySound(_snd_no);
							return;
						}

						Order *order = Engine.Level.Orders.Allocate();
						order->Data = MoveOrder{ mousePos, &action };
						order->Mirage = *m;
						order->Mirage.TilePos = mousePos;

						order->Mirage.MoveActionRemaining -= tileDiff;
						if (order->Mirage.MoveActionRemaining < 0) {
							order->Mirage.StdActionRemaining += order->Mirage.MoveActionRemaining;
							order->Mirage.MoveActionRemaining = 0;
						}

						EnqueueOrder(_selected, order);
					},
					[&](const AttackAction &a) 
					{
						for (auto &u : Engine.Level.Units) {
							if (u.TilePos == mousePos) {
								const auto m = GetLastMirage(_selected);
								const auto d = (u.TilePos - _selected->TilePos).Magnitude();
								if (m->StdActionRemaining < 6 || d > a.Range) {
									Engine.Audio.PlaySound(_snd_no);
									return;
								}

								Order *order = Engine.Level.Orders.Allocate();
								order->Data = AttackOrder{ u.ID(), &a };
								order->Mirage = *m;
								order->Mirage.StdActionRemaining -= 6;

								EnqueueOrder(_selected, order);
							}
						}
					}
				),
				*_selectedAction->Action);
            }
        }
    }

    if (Engine.MousePressed(1) && _selected) {
		const ActionButton *a = std::find_if(_availableActions.begin(), _availableActions.end(), 
			[](const ActionButton &b){
				return static_cast<bool>(std::get_if<MoveAction>(b.Action));
		}).operator->();

		const auto action = std::get_if<MoveAction>(a->Action);

		if (!a) {
			Engine.Audio.PlaySound(_snd_no);
			return; //TODO: not return
		}

		const auto m = GetLastMirage(_selected);
		int tileDiff = std::abs(mousePos.x - m->TilePos.x) +
			std::abs(mousePos.y - m->TilePos.y);

		if (tileDiff > m->MoveActionRemaining + m->StdActionRemaining) {
			Engine.Audio.PlaySound(_snd_no);
			return;
		}

		Order *order = Engine.Level.Orders.Allocate();
		order->Data = MoveOrder{ mousePos, action };
		order->Mirage = *m;
		order->Mirage.TilePos = mousePos;

		order->Mirage.MoveActionRemaining -= tileDiff;
		if (order->Mirage.MoveActionRemaining < 0) {
			order->Mirage.StdActionRemaining += order->Mirage.MoveActionRemaining;
			order->Mirage.MoveActionRemaining = 0;
		}

		EnqueueOrder(_selected, order);
    }
}

void
PlayerController::Draw(Game &Engine)
{
    if (_selected) {
        auto spr =
          Engine.Content.LoadTexture("/content/selected.png")->Sprite();

        Engine.View.DrawSprite(
          spr, GetTileCenter(_selected->TilePos), 0, { 1, 1 }, Colors::White);
    }

    Engine.Screen.DrawRectangle(Rectangle::FromCorner({ 0, 0 }, 1920, 180),
                                Colors::Black);

    auto font = Engine.Content.LoadFont("C:/Windows/fonts/times.ttf", 32);

    Engine.Screen.DrawRectangle(
      Rectangle::FromCorner({ 1920 - 150, 60 }, 120, 60), Colors::Green);
    Engine.Screen.RenderText(
      "End Turn", font, { 1920 - 150, 90 }, { 1, 1 }, Colors::Black);

    if (_selected) {

        for (auto &button : _availableActions) {
            vec4 color = Colors::Red;
            if (_selectedAction == &button)
                color = Colors::Green;

            Engine.Screen.DrawRectangle(button.Box, color);
            Engine.Screen.RenderText(
				std::visit([](const auto &val) -> const std::string & { return val.Name(); }, *button.Action),
              font,
              { button.Box.X - button.Box.HalfWidth / 2, button.Box.Y },
              { 1, 1 },
              Colors::Black);
        }

        const Order *const *orders = &_selected->Orders;
        while (*orders) {
            const Order &o = **orders;

            DrawMirage(&Engine, o.Mirage);
            orders = &o.Next;
        }

		const auto m = GetLastMirage(_selected);
		Engine.Screen.RenderText("Move: " + std::to_string(m->MoveActionRemaining),
			font,
			{ 600, 140 },
			{ 1, 1 },
			Colors::Green);

		Engine.Screen.RenderText("Std:  " + std::to_string(m->StdActionRemaining),
			font,
			{ 600, 100 },
			{ 1, 1 },
			Colors::Red);
    }
}

RenderOrder
PlayerController::RequestedDrawOrder()
{
    return RenderOrder(10);
}
