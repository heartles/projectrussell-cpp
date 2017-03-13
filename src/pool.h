#pragma once

#include <vector>
#include <memory>
#include <array>

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
        using bucket_arr = std::array<inst, BUCKET_SIZE>;
        std::unique_ptr<bucket_arr> data;
        int free;

        bucket()
        {
            data = std::make_unique<bucket_arr>(bucket_arr{});
            free = BUCKET_SIZE;
        }

        bool Contains(T *val)
        {
            return static_cast<void *>(val) > &operator[](0) && static_cast<void *>(val) < &operator[](BUCKET_SIZE-1);
        }

        inst & operator[](int index)
        {
            return (*data)[index];
        }
    };

    std::vector<bucket> _pool{};

  public:
    class it
    {
        friend class Pool<T>;
        Pool<T> *_pool;
        int _bucketIndex, _index;

        it(Pool<T> *p, int b, int i)
        {
            _pool = p;
            _bucketIndex = b;
            _index = i;
        }

    public:
        T& operator*()
        {
            return _pool->_pool[_bucketIndex][_index].val;
        }

        operator T*()
        {
            return &_pool->_pool[_bucketIndex][_index].val;
        }

        T *operator ->()
        {
            return operator T*();
        }

        it operator ++(int)
        {
            auto temp = *this;
            
            do {
                _index++;
                if (_index >= BUCKET_SIZE) {
                    _index = 0;
                    _bucketIndex++;
                }
            } while (_bucketIndex < _pool->_pool.size() &&
                !_pool->_pool[_bucketIndex][_index].inUse);
            
            return temp;
        }
        
        it & operator ++()
        {
            do {
                _index++;
                if (_index >= BUCKET_SIZE) {
                    _index = 0;
                    _bucketIndex++;
                }
            } while (_bucketIndex < _pool->_pool.size()
                && !_pool->_pool[_bucketIndex][_index].inUse);

            return *this;
        }

        bool operator ==(const it& other)
        {
            return _pool == other._pool &&
                _bucketIndex == other._bucketIndex &&
                _index == other._index;
        }

        bool operator !=(const it& other)
        {
            return !(*this == other);
        }
    };

    void Free(T* val)
    {
        for (auto& b : _pool) {

            if (b.Contains(val)) {
                b.free++;

                for (int i = 0; i < BUCKET_SIZE; i++) {
                    if (&b[i].val == val) {
                        b[i].inUse = false;
                        val->~T();
                        return;
                    }
                }
            }
        }
    }

    T* Allocate()
    {
        for (bucket& b : _pool) {
            if (b.free > 0) {
                b.free--;
                
                for (int i = 0; i < BUCKET_SIZE; i++) {
                    if (!b[i].inUse) {
                        b[i].inUse = true;
                        b[i].val = T();
                        return &b[i].val;
                    }
                }
            }
        }

        _pool.emplace_back();
        _pool[_pool.size() - 1][0].inUse = true;
        _pool[_pool.size() - 1][0].val = T();
        return &_pool[_pool.size() - 1][0].val;
    }

    it begin()
    {
        return it{ this, 0, 0 };
    }

    it end()
    {
        return it{ this, static_cast<int>(_pool.size()), 0 };
    }
};
