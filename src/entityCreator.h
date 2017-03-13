#pragma once
#include "game.h"
#include <json/json.h>
#include <string>

struct EntityDesc
{
    std::string Name, Type;
    vec2 Pos;
    vec2 Scale;
    float Rot;
    bool Visible;
    int TileGID;
    Json::Value Properties;
};

using ObjectCreationMap =
  const std::map<std::string, void (*)(Game &, Level *, const EntityDesc &)>;

extern ObjectCreationMap ObjectCreator;