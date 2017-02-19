#include <map>
#include <string>

#include <json/json.h>

#include "entities/player.h"
#include "entityCreator.h"
#include "game.h"

ObjectCreationMap ObjectCreator{
    { "player",
      [](Game& Engine, Level* level, const EntityDesc& obj) {
          Engine.AddComponent(new Player(Engine, *level, obj));
      } },

    { "view",
      [](Game& Engine, Level* level, const EntityDesc& obj) {
          Rectangle v =
            Rectangle::FromCorner(obj.Pos, obj.Scale.x, obj.Scale.y);
          v.Y -= obj.Scale.y;
          Engine.View = v;
      } }
};
