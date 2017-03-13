#include "player.h"

#include <json/json.h>

#include "../mathstrings.h"

// TODO
#include <GLFW/glfw3.h>

ivec2
GetTilePos(const Game& engine, vec2 screen)
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
PlayerController::selectUnit(Unit* u)
{
    _selected = u;
    _selectedAction = nullptr;
    _availableActions.assign(
      { { ActionType::Move, "Move",
          Rectangle::FromCorner({ 60, 60 }, 120, 60) },
        { ActionType::Attack, "Attack",
          Rectangle::FromCorner({ 200, 60 }, 120, 60) } });
}

void
PlayerController::deselectUnit(Unit* u)
{
    _selected = nullptr;
    _selectedAction = nullptr;
    _availableActions.clear();
}

void
EnqueueOrder(Unit* u, Order* o)
{
    Order** valToSet = &u->Orders;
    Order* prevOrder = nullptr;
    while (*valToSet) {
        prevOrder = *valToSet;
        valToSet = &(*valToSet)->Next;
    }

    if (!prevOrder) {
        o->Mirage = Mirage{ u->Spr, u->TilePos, u };
    } else {
        o->Mirage = prevOrder->Mirage;
    }

    switch (o->Type) {
        case ActionType::Move: {
            o->Mirage.TilePos = o->TilePos;
        } break;
    }

    *valToSet = o;
}

void
PlayerController::Update(Game &Engine)
{
    const bool m = Engine.MousePressed(0);
    const auto mousePos =
      GetTilePos(Engine, { Engine.Input.MouseX, Engine.Input.MouseY });
    if (Engine.MousePressed(0)) {
        bool clickedOnUnit = false;
        if (!_selectedAction || _selectedAction->Type == ActionType::None) {
            for (auto& u : Engine.Level.Units) {
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
            for (auto& button : _availableActions) {
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
                ProcessTurn(Engine);
                return;
            }

            if (_selectedAction) {
                switch (_selectedAction->Type) {
                case ActionType::Move: {
                    Order* order = Engine.Level.Orders.Allocate();
                    order->Type = ActionType::Move;
                    order->TilePos = mousePos;
                    EnqueueOrder(_selected, order);
                } break;
                case ActionType::Attack: {
                    for (auto& u : Engine.Level.Units) {
                        if (u.TilePos == mousePos) {
                            Order* order = Engine.Level.Orders.Allocate();
                            order->Type = ActionType::Attack;
                            order->Other = &u;
                            EnqueueOrder(_selected, order);
                        }
                    }
                } break;
                }
            }
        }
    }

    if (Engine.MousePressed(1) && _selected) {
        Order* order = Engine.Level.Orders.Allocate();
        order->Type = ActionType::Move;
        order->TilePos = mousePos;
        EnqueueOrder(_selected, order);
    }
}

void
PlayerController::DrawGUI(Game &Engine)
{
    if (_selected) {
        auto spr =
          Engine.Content.LoadTexture("/content/selected.png")->Sprite();

        Engine.View.DrawSprite(spr, GetTileCenter(_selected->TilePos), 0,
                               { 1, 1 }, Colors::White);
    }

    Engine.Screen.DrawRectangle(Rectangle::FromCorner({ 0, 0 }, 1920, 180),
                                Colors::Black);

    auto font = Engine.Content.LoadFont("C:/Windows/fonts/times.ttf", 32);

    Engine.Screen.DrawRectangle(
      Rectangle::FromCorner({ 1920 - 150, 60 }, 120, 60), Colors::Green);
    Engine.Screen.RenderText("End Turn", font, { 1920 - 150, 90 }, { 1, 1 },
                             Colors::Black);

    if (_selected) {

        for (auto& button : _availableActions) {
            vec4 color = Colors::Red;
            if (_selectedAction == &button)
                color = Colors::Green;

            Engine.Screen.DrawRectangle(button.Box, color);
            Engine.Screen.RenderText(
              button.Name, font,
              { button.Box.X - button.Box.HalfWidth / 2, button.Box.Y },
              { 1, 1 }, Colors::Black);
        }

        const Order * const *orders = &_selected->Orders;
        while (*orders) {
            const Order& o = **orders;

            DrawMirage(&Engine, o.Mirage);
            orders = &o.Next;
        }
    }
}

void
ProcessTurn(Game &Engine)
{
    Log("Processing Turn...");
    for (auto& u : Engine.Level.Units) {
        Order* o = u.Orders;

        Log("\tProcessing orders for unit " + std::to_string(u.ID));
        while (o) {
            switch (o->Type) {
                case ActionType::Move: {
                    Log("\t\tExecuting Move from " +
                        std::to_string(u.TilePos) + " to " +
                        std::to_string(o->TilePos));
                    u.TilePos = o->TilePos;
                } break;
                case ActionType::Attack: {
                    const auto otherUnitID = o->Other->ID;
                    Log("\t\tAttack made against unit " +
                        std::to_string(otherUnitID));
                } break;
            }

            auto nextO = o->Next;
            Engine.Level.Orders.Free(o);
            o = nextO;
        }
        u.Orders = nullptr;
    }

    Log("\tDone processing turn.");
}
