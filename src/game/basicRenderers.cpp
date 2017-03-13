#include "basicRenderers.h"

const int UNIT_LAYER = -10;
const int TILE_LAYER = -100;

void
UnitRenderer::Draw(Game &Engine)
{
    for (auto &u : Engine.Level.Units) {
        DrawUnit(&Engine, u);
    }
}

RenderOrder
UnitRenderer::RequestedDrawOrder()
{
    return RenderOrder(UNIT_LAYER);
}

void
TilemapRenderer::Draw(Game &Engine)
{
    auto shader = Engine.Content.LoadShader("/content/textured.gl.vert",
                                            "/content/textured.gl.frag");

    shader->Apply();

    mat3 viewMat = Engine.View.Matrix();

    for (auto &t : Engine.Level.Tilesets) {
        glBindVertexArray(t.VertexArrayID);

        glBindTexture(GL_TEXTURE_2D, t.Image->TextureID);

        SetUniform("projection", viewMat);
        SetUniform("color", Colors::White);

        glEnableVertexAttribArray(0);

        glEnableVertexAttribArray(1);

        glDrawArrays(GL_TRIANGLES, 0, t.Positions.size());
    }
}

RenderOrder
TilemapRenderer::RequestedDrawOrder()
{
    return RenderOrder(TILE_LAYER);
}
