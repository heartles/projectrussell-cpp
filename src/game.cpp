#include "game.h"

#include <cmath>
#include <exception>
#include <fstream>
#include <iostream>

#include <GLFW/glfw3.h>
#include <json/json.h>

#include "content.h"
#include "game/player.h"
#include "entityCreator.h"
#include "graphics.h"
#include "math.h"
#include "shader.h"
#include "game/unitRenderer.h"

using namespace std;

struct Rectangle
FromPixels(Texture spr, struct Rectangle r)
{
    Rectangle output;
    output.X = r.X / spr.Width;
    output.HalfWidth = r.HalfWidth / spr.Width;
    output.Y = r.Y / spr.Height;
    output.HalfHeight = r.HalfHeight / spr.Height;

    return output;
}

void
Game_Init(Game& info)
{
    Log("creating world");
    info.View = OrthoView(Rectangle{ 0, 0, 30 / 2, 16.875f / 2.0f }, &info);

    StackAlloc alloc(1024 * 1024);

    info.ClearColor = Colors::White;

    auto shader = info.Content.LoadShader("/content/textured.gl.vert",
                                          "/content/textured.gl.frag");

    shader->Apply();

    auto s =
      info.Content.LoadShader("/content/text.gl.vert", "/content/text.gl.frag");

    string fileLoc = info.Content.ResolvePath("/content/main.json");

    LoadLevel(fileLoc, info);
}

void
LoadLevel(const std::string& fileLoc, Game& info)
{
    fstream file;
    file.open(fileLoc);

    Json::Value val;
    file >> val;
    Level& level = info.Level;
    auto tilesetsJson = val["tilesets"];
    level.Tilesets.reserve(tilesetsJson.size());

    level.TileWidth = val["tilewidth"].asInt();
    level.TileHeight = val["tileheight"].asInt();

    level.Width = val["width"].asInt();
    level.Height = val["height"].asInt();

    for (auto tileset : tilesetsJson) {
        Tileset t;

        if (tileset["properties"]["type"] != "tilemap")
            continue;

        auto imageString = tileset["image"].asString();

        auto lastSlash = imageString.find_last_of('/');
        t.ImageName = lastSlash != std::string::npos
                        ? imageString.substr(lastSlash)
                        : "/" + imageString;
        t.ImageHeight = tileset["imageheight"].asInt();
        t.ImageWidth = tileset["imagewidth"].asInt();
        t.TileWidth = tileset["tilewidth"].asInt();
        t.TileHeight = tileset["tileheight"].asInt();
        t.Spacing = tileset["spacing"].asInt();
        t.Margin = tileset["margin"].asInt();

        t.TileCountX =
          (t.ImageWidth + t.Spacing - 2 * t.Margin) / (t.TileWidth + t.Spacing);
        t.TileCountY = (t.ImageHeight + t.Spacing - 2 * t.Margin) /
                       (t.TileHeight + t.Spacing);

        t.TileCountTotal = tileset["tilecount"].asInt();
        t.FirstTileID = tileset["firstgid"].asInt();

        t.Image = info.Content.LoadTexture("/content" + t.ImageName);

        level.Tilesets.push_back(t);
    }

    auto layersJSON = val["layers"];
    for (auto layer : layersJSON) {
        if (layer["type"] == "tilelayer")
            ParseTileLayer(layer, level);
        else if (layer["type"] == "objectgroup") {
            for (auto obj : layer["objects"]) {
                auto halfWidth = obj["width"].asFloat() / 2 / level.TileWidth;
                auto halfHeight =
                  obj["height"].asFloat() / 2 / level.TileHeight;

                auto pos =
                  vec2{ obj["x"].asFloat() / level.TileWidth + halfWidth,
                        -obj["y"].asFloat() / level.TileHeight + halfHeight };

                std::string type = ToLower(obj["type"].asString());

                EntityDesc desc = { obj["name"].asString(),
                                    type,
                                    { obj["x"].asFloat() / level.TileWidth,
                                      -obj["y"].asFloat() / level.TileHeight },
                                    { obj["width"].asFloat() / level.TileWidth,
                                      obj["height"].asFloat() /
                                        level.TileHeight },
                                    obj["rotation"].asFloat(),
                                    obj["visible"].asBool(),
                                    obj["gid"].asInt(),
                                    obj["properties"] };

                auto instantiator = ObjectCreator.find(type);
                if (instantiator != ObjectCreator.end()) {
                    (*instantiator).second(info, &level, desc);
                }
            }
        }
    }

    info.Add(UnitRenderer{});

    for (auto& t : level.Tilesets) {
        glGenVertexArrays(1, &t.VertexArrayID);
        GLenum err = glGetError();
        glBindVertexArray(t.VertexArrayID);

        glGenBuffers(2, t.VertexBufferIDs);

        glBindBuffer(GL_ARRAY_BUFFER, t.VertexBufferIDs[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof_vector(t.Positions),
                     t.Positions.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        glBindBuffer(GL_ARRAY_BUFFER, t.VertexBufferIDs[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof_vector(t.Texcoords),
                     t.Texcoords.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    }

    info.Add(PlayerController{});
}

void
ParseTileLayer(const Json::Value& layer, Level &level)
{
    auto data = layer["data"];
    for (int i = 0; i < data.size(); ++i) {
        auto tile = data[i];

        int tileIndex = tile.asInt();

        if (tileIndex == 0)
            continue;

        Tileset* tilesetPtr = nullptr;

        for (auto& t : level.Tilesets) {
            if (tileIndex - t.FirstTileID >= 0 &&
                tileIndex - t.FirstTileID - t.TileCountTotal < 0) {
                tilesetPtr = &t;
                tileIndex -= t.FirstTileID;
            }
        }

        if (!tilesetPtr) {
            throw std::exception("no matching tileset for tileid");
        }
        auto& tileset = *tilesetPtr;

        float minx = float(i % level.Width) - 0.01f;
        float maxx = float(i % level.Width + 1) + 0.01f;

        float maxy = float(-i / level.Width) + 0.01f;
        float miny = float(-i / level.Width - 1) - 0.01f;

        // tri 1
        tileset.Positions.push_back({
          minx, miny, 1.0f,
        }); // bottom left
        tileset.Positions.push_back({
          maxx, miny, 1.0f,
        }); // bottom right
        tileset.Positions.push_back({
          minx, maxy, 1.0f,
        }); // top left

        // tri 2
        tileset.Positions.push_back({
          maxx, maxy, 1.0f,
        }); // top right
        tileset.Positions.push_back({
          minx, maxy, 1.0f,
        }); // top left
        tileset.Positions.push_back({
          maxx, miny, 1.0f,
        }); // bottom right

        auto coords = tileset.CornerCoordsFromID(tileIndex);

        vec2& tl = coords[0];
        vec2& tr = coords[1];
        vec2& br = coords[2];
        vec2& bl = coords[3];

        // tri 1
        tileset.Texcoords.push_back(bl); // bottom left
        tileset.Texcoords.push_back(br); // Bottom right
        tileset.Texcoords.push_back(tl); // top left

        // tri 2
        tileset.Texcoords.push_back(tr); // top right
        tileset.Texcoords.push_back(tl); // top left
        tileset.Texcoords.push_back(br); // Bottom right
    }
}

void
Game_Update(Game& info)
{
    if (info.Input.Keyboard[GLFW_KEY_ESCAPE])
        info.ShouldClose = true;

    for (auto c : info.Updateables) {
        c->Update(info);
    }
}

void
Game_Render(Game& info)
{
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto shader = info.Content.LoadShader("/content/textured.gl.vert",
                                          "/content/textured.gl.frag");

    shader->Apply();

    mat3 viewMat = // Identity<mat3>();
      Scale({ 2 / info.View.Width(), 2 / info.View.Height() }) *
      Translate({ -info.View.X, -info.View.Y });

    for (auto& t : info.Level.Tilesets) {
        glBindVertexArray(t.VertexArrayID);

        glBindTexture(GL_TEXTURE_2D, t.Image->TextureID);

        SetUniform("projection", viewMat);
        SetUniform("color", Colors::White);

        glEnableVertexAttribArray(0);

        glEnableVertexAttribArray(1);

        glDrawArrays(GL_TRIANGLES, 0, t.Positions.size());
    }

    for (auto c : info.Renderables) {
        c->Draw(info);
    }
    
    for (auto c : info.GUIRenderables) {
        c->DrawGUI(info);
    }

}
