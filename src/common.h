#pragma once

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "math.h"

#define internal_function static
#define internal_variable static
#define countof_array(arr) (sizeof(arr) / sizeof(arr[0]))

namespace stdex {
    template <typename Container, typename Predicate>
    void sort(Container &c, Predicate p)
    {
        return std::sort(c.begin(), c.end(), p);
    }
}

template <typename T>
size_t
sizeof_vector(std::vector<T> vector)
{
    return vector.size() * sizeof(T);
}

void Log(std::string s);

inline std::string
ToLower(const std::string &s)
{
    std::stringstream out{};

    for (const auto c : s) {
        out.put(std::tolower(c));
    }

    return out.str();
}