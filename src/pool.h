#pragma once

#include <vector>

template <typename T, uint32_t BUCKET_SIZE = 64>
class Pool
{
    struct inst
    {
        bool inUse = false;
        T val;
    };

    struct bucket
    {
        inst* data;
        int free;

        inline bucket()
        {
            data = new inst[BUCKET_SIZE];
            free = BUCKET_SIZE;
        }

        inline ~bucket() { delete[] data; }
    };

    std::vector<bucket> _pool{};

  public:
    void Free(T* val)
    {
        for (auto& b : _pool) {
            if ((void*)(val) > b.data && (void*)(val) < b.data + BUCKET_SIZE) {
                b.free++;
                for (int i = 0; i < BUCKET_SIZE; i++) {
                    if (&b.data[i].val == val) {
                        b.data[i].inUse = false;
                        val->~T();
                        return;
                    }
                }
            }
        }
    }

    T* Allocate()
    {
        for (auto& b : _pool) {
            if (b.free > 0) {
                b.free--;
                for (int i = 0; i < BUCKET_SIZE; i++) {
                    if (!b.data[i].inUse) {
                        b.data[i].inUse = true;
                        b.data[i].val = T();
                        return &b.data[i].val;
                    }
                }
            }
        }

        _pool.emplace_back();
        _pool[_pool.size() - 1].data[0].inUse = true;
        _pool[_pool.size() - 1].data[0].val = T();
        return &_pool[_pool.size() - 1].data[0].val;
    }
};