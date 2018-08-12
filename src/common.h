#pragma once

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "math.h"

#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)
#define LINE_STR TO_STRING(__LINE__)
#define CODE_REF_STRING (std::string(__func__) + " (" __FILE__ ":" LINE_STR ")")
#define NOT_IMPLEMENTED() throw std::logic_error("Not Implemented: " + CODE_REF_STRING)

#define internal_function static
#define internal_variable static
#define persistent_variable static
#define countof_array(arr) (sizeof(arr) / sizeof(arr[0]))

// Creates a Strongly Typed Typedef based on type <type>
// with name <name>.
// Requires explicit conversions to convert to/from <type>.
#define STRONG_TYPEDEF(name, type) \
class name \
{ \
	type _t; \
public: \
	inline explicit name(const type &t) \
	{ \
		_t = t; \
	} \
	inline explicit operator type() const \
	{ \
		return _t; \
	} \
	name() = default; \
	name(const name &) = default; \
	name(name &&) = default; \
	name &operator=(const name &) = default; \
	name &operator=(name &&) = default; \
	inline bool operator==(const name &other) {\
		return _t == other._t;\
	}\
};

// Defines equality comparisons for a STRONG_TYPEDEF
#define STRONG_TYPEDEF_EQUALITY(name, type) 

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

void Log(const std::string &s);

inline std::string
ToLower(const std::string &s)
{
    std::stringstream out{};

    for (const auto c : s) {
        out.put(std::tolower(c));
    }

    return out.str();
}