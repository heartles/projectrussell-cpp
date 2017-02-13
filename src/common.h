#pragma once

#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#define internal_function static
#define countof_array(arr) (sizeof(arr) / sizeof(arr[0]))
const float pi = 3.14159f;

typedef unsigned char u8;
typedef signed char s8;

// TODO: remove STL dependencies
struct Input
{
    bool Keyboard[349];
    bool Mouse[8];
    float MouseX, MouseY;
};

template <typename T>
inline size_t
sizeof_vector(std::vector<T> vector)
{
    return vector.size() * sizeof(T);
}

void Log(std::string s);

struct Allocator
{
    virtual void* Alloc(size_t size) = 0;
    virtual void Dealloc(void* data, size_t size) = 0;
    virtual ~Allocator(){};
};

struct StackAlloc : public Allocator
{
    char* _memory;
    size_t _used, _max;

    StackAlloc(size_t max)
    {
        _memory = (char*)std::malloc(max);
        _max = max;
        _used = 0;
    }

    void* Alloc(size_t size) override
    {
        if (_used + size > _max) {
            // TODO
            std::exit(-1);
        }
        auto ret = &(_memory[_used]);
        _used += size;
        return ret;
    }

    void Dealloc(void* data, size_t size) override
    {
        // TODO Error handling
        _used -= size;
    }

    ~StackAlloc() override { std::free(_memory); }
};

inline std::string
ToLower(const std::string& s)
{
    std::stringstream out{};

    for (auto c : s) {
        out.put(std::tolower(c));
    }

    return out.str();
}