#include "player.h"

#include <json/json.h>

// TODO
#include <GLFW/glfw3.h>

ivec2 GetTilePos(Game &engine, vec2 screen)
{
    auto game = ToGame(engine, screen); //TODO: should this be inlined?
    return { int(game.x), int(game.y) };
}

vec2 TilePosToFloat(ivec2 tile)
{
    return { tile.x + 0.5f, tile.y - 0.5f };
}

void
PlayerController::Update()
{
    auto mousePos = GetTilePos(Engine, { Engine.Input.MouseX, Engine.Input.MouseY });
    if (Engine.MousePressed(0)) {
        for (auto &u : Engine.Units) {
            if (u.TilePos == mousePos) {
                if (&u != _selected) {
                    _selected = &u;
                }
                else {
                    _selected = nullptr;
                }
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
         auto spr = Engine.Content.LoadTexture(Engine.GameDir + "/content/selected.png")->Sprite();

        Engine.View.DrawSprite(spr, GetTileCenter(_selected->TilePos), 0, { 1, 1 }, Colors::White);
    }

    Engine.View.DrawRectangleScreen(Rectangle::FromCorner({ 0, 14 }, 32, 4), Colors::Red);
}
