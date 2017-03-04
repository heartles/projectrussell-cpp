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
PlayerController::Update()
{
    bool m = Engine.MousePressed(0);
    auto mousePos =
      GetTilePos(Engine, { Engine.Input.MouseX, Engine.Input.MouseY });
    if (Engine.MousePressed(0)) {
        bool clickedOnUnit = false;
        for (auto& u : Engine.Units) {
            if (u.TilePos == mousePos) {
                if (&u != _selected) {
                    _selected = &u;
                } else {
                    _selected = nullptr;
                }

                clickedOnUnit = true;
            }
        }

        bool clickedOnButton = false;
        if (_selected) {
            vec2 screenMousePos = Engine.Screen.ViewportToWorld(
              { Engine.Input.MouseX, Engine.Input.MouseY });
            if (Rectangle::FromCorner({ 60, 60 }, 120, 60)
                  .Contains(screenMousePos)) {
                if (_selectedAction == ActionType::Move) {
                    _selectedAction = ActionType::None;
                } else {
                    _selectedAction = ActionType::Move;
                }

                clickedOnButton = true;
            }
        }

        if (!clickedOnUnit && !clickedOnButton) {
            switch (_selectedAction) {
                case ActionType::Move: {
                    _selected->TilePos = mousePos;
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

        if (_selectedAction == ActionType::Move)
            Engine.Screen.DrawRectangle(
              Rectangle::FromCorner({ 60, 60 }, 120, 60), Colors::Green);
        else
            Engine.Screen.DrawRectangle(
              Rectangle::FromCorner({ 60, 60 }, 120, 60), Colors::Red);

        Engine.Screen.RenderText("Move", font, { 90, 90 }, { 1, 1 },
                                 Colors::Black);
    }
}
