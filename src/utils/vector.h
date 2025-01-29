#ifndef VECTOR_H
#define VECTOR_H
#include <platform/log.h>
#include <utils/algorithm.h>
#include <utils/new.h>
#include <utils/riterator.h>

namespace LunaVoxelEngine::Utils
{
template<typename T> class Vector final
{
  public:
    // Type aliases
    using value_type = T;
    using size_type = unsigned long;
    using reference = T &;
    using const_reference = const T &;
    using iterator = T *;
    using const_iterator = const T *;
    using reverse_iterator = ReverseIterator<iterator>;
    using const_reverse_iterator = ReverseIterator<const_iterator>;

    // Constructors and destructor
    constexpr Vector() = default;

    constexpr explicit Vector(size_type count, const T &value = T()) noexcept
        : _size{count}
        , _capacity{count}
    {
        if (_size > 0)
        {
            _data = new T[_size];
            for (size_type i = 0; i < _size; i++)
            {
                _data[i] = value;
            }
        }
        else
        {
            _data = nullptr;
        }
    }

    constexpr Vector(const Vector &other) noexcept
        : _size{other._size}
        , _capacity{other._capacity}
    {
        if (_size > 0)
        {
            _data = new T[_size];
            for (size_type i = 0; i < _size; i++)
            {
                _data[i] = other._data[i];
            }
        }
        else
        {
            _data = nullptr;
        }
    }

    constexpr Vector(Vector &&other) noexcept
        : _size{other._size}
        , _capacity{other._capacity}
        , _data{other._data}
    {
        other._data = nullptr;
        other._size = 0;
        other._capacity = 0;
    }

    ~Vector()
    {
        if (_data != nullptr)
        {
            delete[] _data;
        }
    }

    // Assignment operators
    constexpr Vector &operator=(const Vector &other) noexcept
    {
        if (this != &other)
        {
            Vector temp(other);
            swap(temp);
        }
        return *this;
    }
    constexpr Vector &operator=(Vector &&other) noexcept
    {
        if (this != &other)
        {
            swap(other);
        }
        return *this;
    }

    // Capacity
    constexpr size_type size() const noexcept
    {
        return _size;
    }
    constexpr size_type capacity() const noexcept
    {
        return _capacity;
    }
    constexpr bool empty() const noexcept
    {
        return _size == 0;
    }
    void reserve(size_type new_cap) noexcept
    {
        ensure_capacity(new_cap);
    }
    void shrink_to_fit() noexcept
    {
        ensure_capacity(_size);
        _capacity = _size;
    }

    // Element access
    reference operator[](size_type pos) noexcept
    {
        return at(pos);
    }

    const_reference operator[](size_type pos) const noexcept
    {
        return at(pos);
    }

    reference at(size_type pos)
    {
        if (pos >= _size)
        {
            Log::fatal("Index out of range");
        }
        return _data[pos];
    }

    const_reference at(size_type pos) const
    {
        if (pos >= _size)
        {
            Log::fatal("Index out of range");
        }
        return _data[pos];
    }

    iterator begin() noexcept
    {
        return _data;
    }

    const_iterator begin() const noexcept
    {
        return _data;
    }

    const_iterator cbegin() const noexcept
    {
        return _data;
    }

    iterator end() noexcept
    {
        return _data + _size;
    }

    const_iterator end() const noexcept
    {
        return _data + _size;
    }

    const_iterator cend() const noexcept
    {
        return _data + _size;
    }

    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }

    reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }

    reference front() noexcept
    {
        return at(0);
    }

    const_reference front() const noexcept
    {
        return at(0);
    }

    reference back() noexcept
    {
        return at(_size - 1);
    }

    const_reference back() const noexcept
    {
        return at(_size - 1);
    }

    T *data() noexcept
    {
        return _data;
    }

    const T *data() const noexcept
    {
        return _data;
    }

    // Modifiers
    void clear() noexcept
    {
        _size = 0;
        _capacity = 0;
        if (_data != nullptr)
        {
            delete[] _data;
        }
        _data = nullptr;
    }

    void push_back(const T &value)
    {
        ensure_capacity(_size + 1);
        _data[_size++] = value;
    }

    template<typename... Args> void emplace_back(Args &&...args)
    {
        if (_size == _capacity)
        {
            reserve(_capacity == 0 ? 1 : _capacity * 2);
        }
        new (_data + _size) T(static_cast<Args &&>(args)...);
        ++_size;
    }

    void push_back(T &&value)
    {
        ensure_capacity(_size + 1);
        _data[_size++] = value;
    }

    void pop_back()
    {
        if (_size > 0)
        {
            --_size;
        }
    }

    void resize(size_type count, const T &value = T())
    {
        if (count > _size)
        {
            while (_size < count)
            {
                push_back(value);
            }
        }
        else
        {
            while (_size > count)
            {
                pop_back();
            }
        }
    }

    void swap(Vector &other) noexcept
    {
        // Using your custom swap function
        swap(_data, other._data);
        unsigned long temp_size = _size;
        unsigned long temp_capacity = _capacity;

        _size = other._size;
        _capacity = other._capacity;

        other._size = temp_size;
        other._capacity = temp_capacity;
    }

  private:
    T *_data = nullptr;
    unsigned long _size = 0;
    unsigned long _capacity = 0;

    void ensure_capacity(unsigned long new_capacity) noexcept
    {
        if (new_capacity > _capacity)
        {
            unsigned long new_cap = (_capacity == 0) ? 8 : _capacity * 2;
            if (new_cap < new_capacity)
            {
                new_cap = new_capacity;
            }

            T *new_ptr = new T[new_cap];
            if (_data != nullptr && _size > 0)
            {
                memcpy(new_ptr, _data, _size * sizeof(T));
            }
            delete[] _data;
            _data = new_ptr;
            _capacity = new_cap;
        }
    }
};
} // namespace LunaVoxelEngine::Utils
#endif
