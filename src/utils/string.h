#ifndef STRING_H
#define STRING_H
#include <utils/algoritom.h>
#include <utils/new.h>
#include <utils/riterator.h>
namespace LunaVoxalEngine::Utils
{

inline long strlen(const char *str)
{
    long len = 0;
    while (str[len] != 0)
    {
        ++len;
    }
    return len;
}

inline int strcmp(const char *str1, const char *str2)
{
    auto len = strlen(str1);
    auto len2 = strlen(str2);
    if (len < len2)
    {
        return -1;
    }
    if (len > len2)
    {
        return 1;
    }
    return memcmp(str1, str2, len);
}

class ThrowAwayString final
{
  public:
    explicit ThrowAwayString(const char *str);
    ~ThrowAwayString();
    ThrowAwayString(const ThrowAwayString &other) = delete;
    ThrowAwayString &operator=(const ThrowAwayString &other) = delete;
    ThrowAwayString(ThrowAwayString &&other) noexcept
    {
        ptr = other.ptr;
        len = other.len;
        other.ptr = nullptr;
        other.len = 0;
    }
    ThrowAwayString &operator=(ThrowAwayString &&other) noexcept
    {
        ptr = other.ptr;
        len = other.len;
        other.ptr = nullptr;
        other.len = 0;
        return *this;
    }
    const char *c_str() const
    {
        return ptr;
    }

    long size() const
    {
        return len;
    }

  private:
    const char *ptr = nullptr;
    long len = 0;
};

class String final
{
  public:
    using data_type = short;
    using udata_type = unsigned short;
    using iterator = data_type *;
    using const_iterator = const data_type *;
    using const_type = const data_type *;
    using reverse_iterator = ReverseIterator<iterator>;
    using const_reverse_iterator = ReverseIterator<const_iterator>;
    using size_type = unsigned long;
    static constexpr size_type npos = static_cast<size_type>(-1);

    friend String operator+(const char *lhs, const String &rhs)
    {
        return String(lhs) + rhs;
    }

    friend String operator+(const_type lhs, const String &rhs)
    {
        return String(lhs) + rhs;
    }

    String() = default;
    String(const_type str, long len_in = -1);
    String(const char *str, long len_in = -1);
    String(const String &other);
    String(String &&other) noexcept;
    ~String();
    String &operator=(const String &other);
    String &operator=(String &&other) noexcept;
    String &operator=(const_type str);
    String operator+(const char *str) const;
    String operator+(const_type str) const;
    String operator+(const String &str) const;
    iterator begin() noexcept;
    const_iterator begin() const noexcept;
    const_iterator cbegin() const noexcept;
    iterator end() noexcept;
    const_iterator end() const noexcept;
    const_iterator cend() const noexcept;
    reverse_iterator rbegin() noexcept;
    const_reverse_iterator rbegin() const noexcept;
    const_reverse_iterator crbegin() const noexcept;
    reverse_iterator rend() noexcept;
    const_reverse_iterator rend() const noexcept;
    const_reverse_iterator crend() const noexcept;
    size_type size() const noexcept;
    size_type length() const noexcept;
    bool empty() const noexcept;
    size_type capacity() const noexcept;
    void reserve(size_type size);
    constexpr size_type max_size() const noexcept;
    data_type at(size_type index) const;
    data_type &at(size_type index);
    data_type operator[](size_type index) const;
    data_type &operator[](size_type index);
    const_type data() const;
    data_type *data();
    data_type front() const;
    data_type back() const;
    data_type *raw_data();
    const_type raw_data() const;
    void push_back(data_type ch);
    void pop_back();
    void shrink_to_fit();
    void resize(size_type new_size);
    void clear();
    constexpr void resize(size_type n, data_type ch);
    String &append(const char *str);
    String &operator+=(const char *str);
    String &append(const String &other);
    String &operator+=(const String &other);
    String &append(data_type ch);
    String &operator+=(data_type ch);
    String substr(size_type start, size_type end) const;
    constexpr String &insert(size_type pos, const String &str);
    constexpr String &insert(size_type pos, const_type s, size_type n);
    constexpr String &insert(size_type pos, size_type n, data_type c);
    constexpr String &erase(size_type pos = 0, size_type n = npos);
    constexpr size_type find(const String &str, size_type pos = 0) const noexcept;
    constexpr size_type rfind(const String &str, size_type pos = npos) const noexcept;
    constexpr size_type find_first_of(const String &str, size_type pos = 0) const noexcept;
    constexpr size_type find_last_of(const String &str, size_type pos = npos) const noexcept;
    constexpr bool operator<(const String &rhs) const noexcept;
    constexpr bool operator<=(const String &rhs) const noexcept;
    constexpr bool operator>(const String &rhs) const noexcept;
    constexpr bool operator>=(const String &rhs) const noexcept;
    constexpr bool operator==(const String &other) const;
    constexpr bool operator!=(const String &other) const;
    const ThrowAwayString throw_away() const;
    bool start_with(const String &other) const;
    bool end_with(const String &other) const;

  private:
    data_type *ptr = nullptr;
    size_type len = 0;
    size_type _str_capacity = 0;

    enum class Encoding
    {
        ASCII,
        UTF8,
        UTF16_LE,
        UTF16_BE,
        UTF32_LE,
        UTF32_BE
    };
    long get_utf16_length(const char *str, Encoding encoding);
    Encoding detect_encoding(const char *str, long len_in);
    Encoding detect_utf8_heuristic(const char *str, long len_in);
    void convert_to_utf16(const char *str, long len_in, Encoding encoding);
    void convert_utf8_to_utf16(const char *str, long len_in);
    void convert_utf16le_to_utf16(const char *str, long len_in);
    void convert_utf16be_to_utf16(const char *str, long len_in);
    void convert_utf32le_to_utf16(const char *str, long len_in);
    void convert_utf32be_to_utf16(const char *str, long len_in);
    void ensure_capacity(size_type new__str_capacity);
};
} // namespace LunaVoxalEngine::Utils
#endif
