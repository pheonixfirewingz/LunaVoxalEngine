#ifndef STRING_H
#define STRING_H
#include <utils/algoritom.h>
#include <utils/new.h>
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
  return memcmp(str1, str2,len); 
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

  private:
    const char *ptr = nullptr;
    long len = 0;
};

class String final
{
  public:
    // Constructors
    String() = default;
    String(const short *str, long len_in = -1);
    String(const char *str, long len_in = -1);
    String(const String &other);
    String(String &&other);
    ~String();

    String &operator=(const String &other);
    String &operator=(String &&other);
    String &operator=(const short *str);
    String operator+(const char *str);
    String operator+(const short *str);
    String operator+(const String &str);
    friend String operator+(const char *lhs, const String &rhs)
    {
        return String(lhs) + rhs;
    }

    friend String operator+(const short *lhs, const String &rhs)
    {
        return String(lhs) + rhs;
    }

    unsigned long size() const;
    unsigned long length() const;
    bool empty() const;
    unsigned long capacity() const;
    void reserve(unsigned long size);
    short at(unsigned long index) const;
    short &at(unsigned long index);
    short operator[](unsigned long index) const;
    short &operator[](unsigned long index);
    const short *data() const;
    short *data();
    short front() const;
    short back() const;
    short *raw_data();
    const short *raw_data() const;
    void push_back(short ch);
    void pop_back();
    void shrink_to_fit();
    void resize(unsigned long new_size);
    String &append(const char *str);
    String &operator+=(const char *str);
    String &append(const String &other);
    String &operator+=(const String &other);
    String &append(short ch);
    String &operator+=(short ch);
    String substr(unsigned long start, unsigned long end) const;
    bool operator==(const String &other) const;
    bool operator!=(const String &other) const;
    const ThrowAwayString throw_away() const;
    bool start_with(const String &other) const;
    bool end_with(const String &other) const;
    void clear();
  private:
    short *ptr = nullptr;
    unsigned long len = 0;
    unsigned long _str_capacity = 0;

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
    void ensure_capacity(unsigned long new__str_capacity);
};
} // namespace LunaVoxalEngine::Utils
#endif
