#ifndef ALGORITHM_H
#define ALGORITHM_H

namespace LunaVoxelEngine
{
namespace Utils
{
template<typename T> constexpr T clamp(T value, T min, T max) noexcept
{
    return value < min ? min : (value > max ? max : value);
}

template<typename T> constexpr T lerp(T a, T b, float t) noexcept
{
    return a + (b - a) * t;
}

template<typename T> constexpr T min(T a, T b) noexcept
{
    return a < b ? a : b;
}

template<typename T> constexpr T max(T a, T b) noexcept
{
    return a > b ? a : b;
}

template<typename T> constexpr T abs(T a) noexcept
{
    return a < 0 ? -a : a;
}

template<typename T> constexpr T sign(T a) noexcept
{
    return a < 0 ? -1 : 1;
}

template<typename T> constexpr void swap(T &a, T &b) noexcept
{
    T tmp = a;
    a = b;
    b = tmp;
}

template<typename Iterator, typename Compare> Iterator partition(Iterator beg, Iterator end, Compare cmp)
{
    // Using the last element as the pivot
    Iterator pivot = end - 1;
    Iterator i = beg - 1;

    for (Iterator j = beg; j < pivot; ++j)
    {
        if (cmp(*j, *pivot))
        { // If element is smaller than the pivot
            ++i;
            swap(*i, *j); // Swap elements using custom my_swap
        }
    }

    // Place the pivot element in its correct sorted position
    swap(*(i + 1), *pivot);
    return i + 1; // Return the pivot's new position
}

template<typename Iterator, typename Compare> void quicksort(Iterator beg, Iterator end, Compare cmp)
{
    if (beg >= end)
        return;

    // Partition the range [beg, end) and get the pivot element's position
    Iterator pivot = partition(beg, end, cmp);

    // Recursively sort the two halves
    quicksort(beg, pivot, cmp);     // Sort the left half
    quicksort(pivot + 1, end, cmp); // Sort the right half
}

inline int memcmp(const void *aptr, const void *bptr, unsigned long size)
{
    const unsigned char *a = (const unsigned char *)aptr;
    const unsigned char *b = (const unsigned char *)bptr;
    for (unsigned long i = 0; i < size; i++)
    {
        if (a[i] < b[i])
            return -1;
        else if (b[i] < a[i])
            return 1;
    }
    return 0;
}

inline void *memcpy(void *dstptr, const void *srcptr, unsigned long size)
{
    if (!dstptr || !srcptr)
        return nullptr; // Check for null pointers
    unsigned char *dst = (unsigned char *)dstptr;
    const unsigned char *src = (const unsigned char *)srcptr;
    for (unsigned long i = 0; i < size; i++)
        dst[i] = src[i];
    return dstptr;
}


inline void *memset(void *bufptr, int value, unsigned long size)
{
    unsigned char *buf = (unsigned char *)bufptr;
    for (unsigned long i = 0; i < size; i++)
        buf[i] = (unsigned char)value;
    return bufptr;
}

inline void *memmove(void *dstptr, const void *srcptr, unsigned long size)
{
    unsigned char *dst = (unsigned char *)dstptr;
    const unsigned char *src = (const unsigned char *)srcptr;
    if (dst < src)
    {
        for (unsigned long i = 0; i < size; i++)
            dst[i] = src[i];
    }
    else
    {
        for (unsigned long i = size; i != 0; i--)
            dst[i - 1] = src[i - 1];
    }
    return dstptr;
}
} // namespace Utils
} // namespace LunaVoxelEngine
#endif
