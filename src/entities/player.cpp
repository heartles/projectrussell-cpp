#include "player.h"

#include <json/json.h>

// TODO
#include <GLFW/glfw3.h>

ivec2
GetTilePos(Game& engine, vec2 screen)
{
    auto game = engine.View.ViewportToWorld(screen);
    return { int(game.x), int(game.y) };
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
    _availableActions.assign({
        { ActionType::Move, "Move", Rectangle::FromCorner({ 60, 60 }, 120, 60) },
        { ActionType::Attack, "Attack", Rectangle::FromCorner({ 200, 60 }, 120, 60) }
    });
}

void
PlayerController::deselectUnit(Unit *u)
{
    _selected = nullptr;
    _selectedAction = nullptr;
    _availableActions.clear();
}

void
PlayerController::Update()
{
    bool m = Engine.MousePressed(0);
    auto mousePos =
      GetTilePos(Engine, { Engine.Input.MouseX, Engine.Input.MouseY });
    if (Engine.MousePressed(0)) {
        bool clickedOnUnit = false;
        if (!_selectedAction || _selectedAction->Type == ActionType::None) {
            for (auto &u : Engine.Units) {
                if (u.TilePos == mousePos) {
                    if (&u != _selected) {
                        selectUnit(&u);
                    }
                    else {
                        deselectUnit(_selected);
                    }

                    clickedOnUnit = true;
                }
            }
        }

        bool clickedOnButton = false;
        if (_selected) {
            vec2 screenMousePos = Engine.Screen.ViewportToWorld(
              { Engine.Input.MouseX, Engine.Input.MouseY });
            
            for (auto &button : _availableActions) {
                if (button.Box.Contains(screenMousePos)) {
                    if (_selectedAction == &button) {
                        _selectedAction = nullptr;
                    }
                    else {
                        _selectedAction = &button;
                    }

                    clickedOnButton = true;
                }
            }
        }

        if (!clickedOnUnit && !clickedOnButton && _selectedAction) {
            switch (_selectedAction->Type) {
                case ActionType::Move: {
                    _selected->TilePos = mousePos;
                } break;
                case ActionType::Attack: {
                    for (auto u : Engine.Units) {
                        if (u.TilePos == mousePos) {
                            Log("Attack made!");
                        }
                    }
                } break;
            }
        }
    }

    if (Engine.MousePressed(1) && _selected) {
        _selected->TilePos = mousePos;
    }
}

void
PlayerController::DrawGUI()
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

    if (_selected) {

        for (auto &button : _availableActions) {
            vec4 color = Colors::Red;
            if (_selectedAction == &button)
                color = Colors::Green;

            Engine.Screen.DrawRectangle(button.Box, color);
            Engine.Screen.RenderText(button.Name, font, 
                { button.Box.X - button.Box.HalfWidth/2 , button.Box.Y }, 
                { 1, 1 }, Colors::Black);
        }
    }
}
