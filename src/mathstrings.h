#pragma once

#include <string>
#include "math.h"

namespace std {
    inline std::string
        to_string(const ivec2& val)
    {
        return "{ivec2 " + std::to_string(val.x) + ", " + std::to_string(val.y) +
            "}";
    }
}