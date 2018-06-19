#include <map>
#include <string>

#include <json/json.h>

#include "entityCreator.h"
#include "game.h"
#include "game/player.h"

ObjectCreationMap ObjectCreator{
    { "unit",
      [](Game &Engine, Level *level, const EntityDesc &obj) {
        throw std::logic_error("Not Implemented");
        /*
          *Engine.Level.Units.Allocate() = Unit(
            level->GetTilesetFromGID(obj.TileGID)->SpriteFromGID(obj.TileGID),
            ivec2(obj.Pos));*/
      } },

    { "view",
      [](Game &Engine, Level *, const EntityDesc &obj) {
          Rectangle v =
            Rectangle::FromCorner(obj.Pos, obj.Scale.x, obj.Scale.y);
          v.Y -= obj.Scale.y;
          Engine.View = OrthoView(v, &Engine);
      } }
};
