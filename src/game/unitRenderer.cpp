#include "unitRenderer.h"

void UnitRenderer::Draw(Game &Engine)
{
    for (auto &u : Engine.Level.Units) {
        DrawUnit(&Engine, u);
    }
}
