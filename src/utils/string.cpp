#include <platform/log.h>
#include <utils/algorithm.h>
#include <utils/string.h>

namespace LunaVoxelEngine::Utils
{
ThrowAwayString::ThrowAwayString(const char *str)
{
    len = strlen(str);
    ptr = new char[len + 1];
    memset(static_cast<void *>(const_cast<char *>(ptr)), '\0', len + 1);
    memcpy(static_cast<void *>(const_cast<char *>(ptr)), reinterpret_cast<const void *>(str), len);
}
ThrowAwayString::~ThrowAwayString()
{
    if (ptr != nullptr)
        delete[] ptr;
    ptr = nullptr;
}

String::String(String::const_type str, long len_in)
{
    if (len_in == -1)
    {
        while (str[len] != 0)
        {
            ++len;
        }
    }
    else
    {
        len = len_in;
    }
    ensure_capacity(len);
    memcpy(ptr, str, len * sizeof(String::data_type));
}

String::String(const char *str, long len_in)
{
    String::Encoding encoding = detect_encoding(str, len_in);
    convert_to_utf16(str, len_in, encoding);
}

String::String(const String &other)
    : len(other.len)
    , _str_capacity(other._str_capacity)
{
    if (len > 0)
    {
        ptr = new String::data_type[_str_capacity];
        memcpy(ptr, other.ptr, len * sizeof(String::data_type));
    }
}

String::String(String &&other) noexcept
    : ptr(other.ptr)
    , len(other.len)
    , _str_capacity(other._str_capacity)
{
    other.ptr = nullptr;
    other.len = 0;
    other._str_capacity = 0;
}

String::~String()
{
    delete[] ptr;
}

String &String::operator=(const String &other)
{
    if (this != &other)
    {
        delete[] ptr;
        len = other.len;
        _str_capacity = other._str_capacity;
        ptr = new String::data_type[_str_capacity];
        memcpy(ptr, other.ptr, len * sizeof(String::data_type));
    }
    return *this;
}

String &String::operator=(String &&other) noexcept
{
    if (this != &other)
    {
        delete[] ptr;
        ptr = other.ptr;
        len = other.len;
        _str_capacity = other._str_capacity;
        other.ptr = nullptr;
        other.len = 0;
        other._str_capacity = 0;
    }
    return *this;
}

String &String::operator=(String::const_type str)
{
    len = strlen((const char *)str);
    ensure_capacity(len);
    memcpy(ptr, str, len * sizeof(String::data_type));
    return *this;
}

String String::operator+(const char *str) const
{
    String result = *this;
    result.append(str);
    return result;
}

String String::operator+(const_type str) const
{
    String result = *this;
    result.append(str);
    return result;
}

String String::operator+(const String &str) const
{
    String result = *this;
    result.append(str);
    return result;
}

String::iterator String::begin() noexcept
{
    return ptr;
}
String::const_iterator String::begin() const noexcept
{
    return ptr;
}
String::const_iterator String::cbegin() const noexcept
{
    return ptr;
}
String::iterator String::end() noexcept
{
    return ptr + len;
}
String::const_iterator String::end() const noexcept
{
    return ptr + len;
}
String::const_iterator String::cend() const noexcept
{
    return ptr + len;
}

String::reverse_iterator String::rbegin() noexcept
{
    return reverse_iterator(end());
}
String::const_reverse_iterator String::rbegin() const noexcept
{
    return const_reverse_iterator(end());
}
String::const_reverse_iterator String::crbegin() const noexcept
{
    return const_reverse_iterator(end());
}
String::reverse_iterator String::rend() noexcept
{
    return reverse_iterator(begin());
}
String::const_reverse_iterator String::rend() const noexcept
{
    return const_reverse_iterator(begin());
}
String::const_reverse_iterator String::crend() const noexcept
{
    return const_reverse_iterator(begin());
}

String::size_type String::size() const noexcept
{
    return len;
}

String::size_type String::length() const noexcept
{
    return len;
}

constexpr bool String::empty() const noexcept
{
    return len == 0;
}

String::size_type String::capacity() const noexcept
{
    return _str_capacity;
}

void String::reserve(String::size_type new__str_capacity)
{
    ensure_capacity(new__str_capacity);
}

constexpr String::size_type String::max_size() const noexcept
{
    return static_cast<size_type>(-1) / sizeof(data_type);
}

String::data_type String::at(String::size_type index) const
{
    if (index >= len)
    {
        Log::fatal("Index out of range: %s", ptr);
    }
    return ptr[index];
}

String::data_type &String::at(String::size_type index)
{
    if (index >= len)
    {
        Log::fatal("Index out of range: %s", ptr);
    }
    return ptr[index];
}

String::data_type String::operator[](String::size_type index) const
{
    return at(index);
}

String::data_type &String::operator[](String::size_type index)
{
    return at(index);
}

String::data_type *String::data()
{
    return ptr;
}

String::const_type String::data() const
{
    return ptr;
}

String::data_type String::front() const
{
    return ptr[0];
}

String::data_type String::back() const
{
    return ptr[len - 1];
}

String::data_type *String::raw_data()
{
    return ptr;
}
String::const_type String::raw_data() const
{
    return ptr;
}

void String::push_back(String::data_type ch)
{
    ensure_capacity(len + 1);
    ptr[len++] = ch;
}

void String::pop_back()
{
    if (len > 0)
    {
        --len;
    }
}

void String::shrink_to_fit()
{
    _str_capacity = len;
    String::data_type *new_ptr = new String::data_type[_str_capacity];
    memcpy(new_ptr, ptr, len * sizeof(String::data_type));
    delete[] ptr;
    ptr = new_ptr;
}

void String::resize(String::size_type new_size)
{
    ensure_capacity(new_size);
    len = new_size;
}

String &String::append(const char *str)
{
    auto encoding = detect_encoding(str, -1);
    convert_to_utf16(str, -1, encoding);
    return *this;
}

String &String::operator+=(const char *str)
{
    return append(str);
}

String &String::append(const String &other)
{
    ensure_capacity(len + other.len);
    memcpy(ptr + len, other.ptr, other.len * sizeof(String::data_type));
    len += other.len;
    return *this;
}

String &String::operator+=(const String &other)
{
    return append(other);
}

String &String::append(String::data_type ch)
{
    ensure_capacity(len + 1);
    ptr[len++] = ch;
    return *this;
}

String &String::operator+=(String::data_type ch)
{
    return append(ch);
}

String String::substr(String::size_type start, String::size_type end) const
{
    String substr;
    substr.reserve(end - start);
    for (String::size_type i = start; i < end; ++i)
    {
        substr.push_back(ptr[i]);
    }
    return substr;
}

String &String::insert(size_type pos, const String &str)
{
    if (pos > len)
    {
        Log::fatal("Index out of range");
    }
    size_type new_len = len + str.len;
    ensure_capacity(new_len);

    // Move existing characters
    for (size_type i = len; i > pos; --i)
    {
        ptr[i + str.len - 1] = ptr[i - 1];
    }

    // Copy new string
    for (size_type i = 0; i < str.len; ++i)
    {
        ptr[pos + i] = str.ptr[i];
    }

    len = new_len;
    return *this;
}

String &String::insert(size_type pos, const_type s, size_type n)
{
    if (pos > len)
    {
        Log::fatal("Index out of range");
    }
    ensure_capacity(len + n);

    // Move existing characters
    for (size_type i = len; i > pos; --i)
    {
        ptr[i + n - 1] = ptr[i - 1];
    }

    // Copy new characters
    for (size_type i = 0; i < n; ++i)
    {
        ptr[pos + i] = s[i];
    }

    len += n;
    return *this;
}

String &String::insert(size_type pos, size_type n, String::data_type c)
{
    if (pos > len)
    {
        Log::fatal("Index out of range");
    }
    ensure_capacity(len + n);

    // Move existing characters
    for (size_type i = len; i > pos; --i)
    {
        ptr[i + n - 1] = ptr[i - 1];
    }

    // Fill with character
    for (size_type i = 0; i < n; ++i)
    {
        ptr[pos + i] = c;
    }

    len += n;
    return *this;
}

String &String::erase(size_type pos, size_type n)
{
    if (pos > len)
    {
        Log::fatal("Index out of range");
    }

    if (n == npos || pos + n > len)
    {
        n = len - pos;
    }

    for (size_type i = pos; i + n < len; ++i)
    {
        ptr[i] = ptr[i + n];
    }

    len -= n;
    return *this;
}

constexpr String::size_type String::find(const String &str, size_type pos) const noexcept
{
    if (pos >= len)
        return npos;
    if (str.empty())
        return pos;

    for (size_type i = pos; i <= len - str.len; ++i)
    {
        bool found = true;
        for (size_type j = 0; j < str.len; ++j)
        {
            if (ptr[i + j] != str.ptr[j])
            {
                found = false;
                break;
            }
        }
        if (found)
            return i;
    }
    return npos;
}

constexpr String::size_type String::rfind(const String &str, size_type pos) const noexcept
{
    if (str.empty())
        return min(pos, len);
    if (len < str.len)
        return npos;

    pos = min(pos, len - str.len);

    for (size_type i = pos + 1; i-- > 0;)
    {
        bool found = true;
        for (size_type j = 0; j < str.len; ++j)
        {
            if (ptr[i + j] != str.ptr[j])
            {
                found = false;
                break;
            }
        }
        if (found)
            return i;
    }
    return npos;
}

constexpr String::size_type String::find_first_of(const String &str, size_type pos) const noexcept
{
    for (size_type i = pos; i < len; ++i)
    {
        for (size_type j = 0; j < str.len; ++j)
        {
            if (ptr[i] == str.ptr[j])
            {
                return i;
            }
        }
    }
    return npos;
}

constexpr String::size_type String::find_last_of(const String &str, size_type pos) const noexcept
{
    if (empty())
        return npos;
    pos = min(pos, len - 1);

    for (size_type i = pos + 1; i-- > 0;)
    {
        for (size_type j = 0; j < str.len; ++j)
        {
            if (ptr[i] == str.ptr[j])
            {
                return i;
            }
        }
    }
    return npos;
}

constexpr bool String::operator<(const String &rhs) const noexcept
{
    size_type min_len = min(len, rhs.len);
    for (size_type i = 0; i < min_len; ++i)
    {
        if (ptr[i] < rhs.ptr[i])
            return true;
        if (ptr[i] > rhs.ptr[i])
            return false;
    }
    return len < rhs.len;
}

constexpr bool String::operator<=(const String &rhs) const noexcept
{
    return !(rhs < *this);
}

constexpr bool String::operator>(const String &rhs) const noexcept
{
    return rhs < *this;
}

constexpr bool String::operator>=(const String &rhs) const noexcept
{
    return !(*this < rhs);
}

constexpr bool String::operator==(const String &other) const
{
    if (len != other.len)
    {
        return false;
    }
    return memcmp(ptr, other.ptr, len * sizeof(String::data_type)) == 0;
}

constexpr bool String::operator!=(const String &other) const
{
    return !(*this == other);
}

const ThrowAwayString String::throw_away() const
{
    if (!ptr)
        return ThrowAwayString("\0");
    char *utf8_result = new char[len + 1];
    memset(utf8_result, '\0', len + 1);

    String::size_type i = 0;
    long j = 0;

    while (i < len)
    {
        String::udata_type code_unit = ptr[i++];

        if (code_unit < 0x80)
        {
            // 1-byte UTF-8
            utf8_result[j++] = static_cast<char>(code_unit);
        }
        else if (code_unit < 0x800)
        {
            // 2-byte UTF-8
            utf8_result[j++] = static_cast<char>(0xC0 | (code_unit >> 6));
            utf8_result[j++] = static_cast<char>(0x80 | (code_unit & 0x3F));
        }
        else if (code_unit >= 0xD800 && code_unit <= 0xDBFF)
        {
            // Surrogate pair: high surrogate
            String::udata_type high_surrogate = code_unit;
            String::udata_type low_surrogate = ptr[i];

            if (low_surrogate >= 0xDC00 && low_surrogate <= 0xDFFF)
            {
                // Valid surrogate pair
                i++; // Consume the low surrogate
                unsigned int code_point = 0x10000 + ((high_surrogate - 0xD800) << 10) + (low_surrogate - 0xDC00);

                // Encode the code point as 4-byte UTF-8
                utf8_result[j++] = static_cast<char>(0xF0 | (code_point >> 18));
                utf8_result[j++] = static_cast<char>(0x80 | ((code_point >> 12) & 0x3F));
                utf8_result[j++] = static_cast<char>(0x80 | ((code_point >> 6) & 0x3F));
                utf8_result[j++] = static_cast<char>(0x80 | (code_point & 0x3F));
            }
            else
            {
                // Invalid surrogate pair; skip the invalid sequence
                continue;
            }
        }
        else if (code_unit >= 0xDC00 && code_unit <= 0xDFFF)
        {
            // Unpaired low surrogate; skip it
            continue;
        }
        else
        {
            // 3-byte UTF-8
            utf8_result[j++] = static_cast<char>(0xE0 | (code_unit >> 12));
            utf8_result[j++] = static_cast<char>(0x80 | ((code_unit >> 6) & 0x3F));
            utf8_result[j++] = static_cast<char>(0x80 | (code_unit & 0x3F));
        }
    }
    ThrowAwayString ret(utf8_result);
    delete[] utf8_result;
    return ret;
}

bool String::start_with(const String &other) const
{
    if (len < other.len)
    {
        return false;
    }
    return memcmp(ptr, other.ptr, other.len * sizeof(String::data_type)) == 0;
}

bool String::end_with(const String &other) const
{
    if (len < other.len)
    {
        return false;
    }
    return memcmp(ptr + len - other.len, other.ptr, other.len * sizeof(String::data_type)) == 0;
}

void String::clear()
{
    delete[] ptr;
    ptr = nullptr;
    len = 0;
    _str_capacity = 0;
}

constexpr void String::resize(size_type n, String::data_type ch)
{
    if (n > len)
    {
        reserve(n);
        while (len < n)
        {
            push_back(ch);
        }
    }
    else
    {
        resize(n);
    }
}

long String::get_utf16_length(const char *str, String::Encoding encoding)
{
    if (!str)
        return 0;

    // Skip BOM
    if (encoding == String::Encoding::UTF8 && static_cast<unsigned char>(str[0]) == 0xEF &&
        static_cast<unsigned char>(str[1]) == 0xBB && static_cast<unsigned char>(str[2]) == 0xBF)
    {
        str += 3;
    }
    else if ((encoding == String::Encoding::UTF16_BE || encoding == String::Encoding::UTF16_LE) &&
             static_cast<unsigned char>(str[0]) == 0xFE && static_cast<unsigned char>(str[1]) == 0xFF)
    {
        str += 2;
    }
    else if ((encoding == String::Encoding::UTF32_BE || encoding == String::Encoding::UTF32_LE) &&
             ((static_cast<unsigned char>(str[0]) == 0x00 && static_cast<unsigned char>(str[1]) == 0x00 &&
               static_cast<unsigned char>(str[2]) == 0xFE && static_cast<unsigned char>(str[3]) == 0xFF) ||
              (static_cast<unsigned char>(str[0]) == 0xFF && static_cast<unsigned char>(str[1]) == 0xFE &&
               static_cast<unsigned char>(str[2]) == 0x00 && static_cast<unsigned char>(str[3]) == 0x00)))
    {
        str += 4;
    }

    long utf16_len = 0;

    switch (encoding)
    {
    case String::Encoding::ASCII:
        utf16_len = strlen(str);
        break;

    case String::Encoding::UTF8: {
        for (long i = 0; str[i];)
        {
            unsigned char byte = static_cast<unsigned char>(str[i]);
            if (byte < 0x80)
            {
                utf16_len++;
                i++;
            }
            else if ((byte & 0xE0) == 0xC0)
            {
                utf16_len++;
                i += 2;
            }
            else if ((byte & 0xF0) == 0xE0)
            {
                utf16_len++;
                i += 3;
            }
            else if ((byte & 0xF8) == 0xF0)
            {
                utf16_len += 2; // Emoji or supplementary character (surrogate pair)
                i += 4;
            }
            else
            {
                // Handle invalid byte sequences gracefully
                i++;
            }
        }
        break;
    }

    case String::Encoding::UTF16_LE:
    case String::Encoding::UTF16_BE: {
        const String::udata_type *ptr = reinterpret_cast<const String::udata_type *>(str);
        while (*ptr)
        {
            if (*ptr >= 0xD800 && *ptr <= 0xDBFF)
            {
                utf16_len += 2; // High surrogate followed by low surrogate
                ptr += 2;
            }
            else
            {
                utf16_len++;
                ptr++;
            }
        }
        break;
    }

    case String::Encoding::UTF32_LE:
    case String::Encoding::UTF32_BE: {
        const unsigned int *utf32_str = reinterpret_cast<const unsigned int *>(str);
        while (*utf32_str)
        {
            utf16_len += (*utf32_str > 0xFFFF) ? 2 : 1; // Surrogate pairs for supplementary planes
            utf32_str++;
        }
        break;
    }
    }

    return utf16_len;
}

String::Encoding String::detect_encoding(const char *str, long len_in)
{
    // Determine actual length if not provided
    if (len_in == -1)
    {
        len_in = strlen(str);
    }

    if (len_in < 2)
        return String::Encoding::ASCII;

    const unsigned char *bytes = reinterpret_cast<const unsigned char *>(str);

    // UTF-8 BOM check
    if (len_in >= 3 && bytes[0] == 0xEF && bytes[1] == 0xBB && bytes[2] == 0xBF)
        return String::Encoding::UTF8;

    // UTF-16 BOM check
    if (len_in >= 2)
    {
        if (bytes[0] == 0xFE && bytes[1] == 0xFF)
            return String::Encoding::UTF16_BE;
        if (bytes[0] == 0xFF && bytes[1] == 0xFE)
            return String::Encoding::UTF16_LE;
    }

    // UTF-32 BOM check
    if (len_in >= 4)
    {
        if (bytes[0] == 0x00 && bytes[1] == 0x00 && bytes[2] == 0xFE && bytes[3] == 0xFF)
            return String::Encoding::UTF32_BE;
        if (bytes[0] == 0xFF && bytes[1] == 0xFE && bytes[2] == 0x00 && bytes[3] == 0x00)
            return String::Encoding::UTF32_LE;
    }

    return detect_utf8_heuristic(str, len_in);
}

String::Encoding String::detect_utf8_heuristic(const char *str, long len_in)
{
    String::size_type utf8_sequence_count = 0;
    String::size_type expected_continuation_bytes = 0;

    for (long i = 0; i < len_in; ++i)
    {
        unsigned char byte = static_cast<unsigned char>(str[i]);

        if (expected_continuation_bytes == 0)
        {
            if (byte <= 0x7F)
                continue;

            if ((byte & 0xE0) == 0xC0)
                expected_continuation_bytes = 1;
            else if ((byte & 0xF0) == 0xE0)
                expected_continuation_bytes = 2;
            else if ((byte & 0xF8) == 0xF0)
                expected_continuation_bytes = 3;
            else
                return String::Encoding::ASCII;

            utf8_sequence_count++;
        }
        else
        {
            if ((byte & 0xC0) != 0x80)
                return String::Encoding::ASCII;
            expected_continuation_bytes--;
        }
    }

    return (utf8_sequence_count > 0) ? String::Encoding::UTF8 : String::Encoding::ASCII;
}

void String::convert_to_utf16(const char *str, long len_in, String::Encoding encoding)
{
    if (len_in == -1)
    {
        len_in = strlen(str);
    }

    ensure_capacity(len_in);
    len = 0;

    switch (encoding)
    {
    case String::Encoding::ASCII:
        for (long i = 0; i < len_in; ++i)
        {
            ptr[len++] = static_cast<String::data_type>(str[i]);
        }
        break;

    case String::Encoding::UTF8:
        convert_utf8_to_utf16(str, len_in);
        break;

    case String::Encoding::UTF16_LE:
        convert_utf16le_to_utf16(str, len_in);
        break;

    case String::Encoding::UTF16_BE:
        convert_utf16be_to_utf16(str, len_in);
        break;

    case String::Encoding::UTF32_LE:
        convert_utf32le_to_utf16(str, len_in);
        break;

    case String::Encoding::UTF32_BE:
        convert_utf32be_to_utf16(str, len_in);
        break;
    }
}

void String::convert_utf8_to_utf16(const char *str, long len_in)
{
    String::size_type utf16_index = 0;
    for (long i = 0; i < len_in;)
    {
        unsigned char first_byte = static_cast<unsigned char>(str[i]);

        if (first_byte < 0x80)
        {
            ensure_capacity(utf16_index + 1);
            ptr[utf16_index++] = first_byte;
            i++;
        }
        else if ((first_byte & 0xE0) == 0xC0)
        {
            if (i + 1 >= len_in)
                Log::fatal("Incomplete UTF-8 sequence");

            unsigned char second_byte = static_cast<unsigned char>(str[i + 1]);
            if ((second_byte & 0xC0) != 0x80)
                Log::fatal("Invalid UTF-8 sequence");

            unsigned int code_point = ((first_byte & 0x1F) << 6) | (second_byte & 0x3F);
            ensure_capacity(utf16_index + 1);
            ptr[utf16_index++] = static_cast<String::data_type>(code_point);
            i += 2;
        }

        else if ((first_byte & 0xF0) == 0xE0)
        {
            if (i + 2 >= len_in)
                Log::fatal("Incomplete UTF-8 sequence");

            unsigned char second_byte = static_cast<unsigned char>(str[i + 1]);
            unsigned char third_byte = static_cast<unsigned char>(str[i + 2]);

            if ((second_byte & 0xC0) != 0x80 || (third_byte & 0xC0) != 0x80)
                Log::fatal("Invalid UTF-8 sequence");

            unsigned int code_point = ((first_byte & 0x0F) << 12) | ((second_byte & 0x3F) << 6) | (third_byte & 0x3F);
            ensure_capacity(utf16_index + 1);
            ptr[utf16_index++] = static_cast<String::data_type>(code_point);
            i += 3;
        }

        else if ((first_byte & 0xF8) == 0xF0)
        {
            if (i + 3 >= len_in)
                Log::fatal("Incomplete UTF-8 sequence");

            unsigned char second_byte = static_cast<unsigned char>(str[i + 1]);
            unsigned char third_byte = static_cast<unsigned char>(str[i + 2]);
            unsigned char fourth_byte = static_cast<unsigned char>(str[i + 3]);

            if ((second_byte & 0xC0) != 0x80 || (third_byte & 0xC0) != 0x80 || (fourth_byte & 0xC0) != 0x80)
                Log::fatal("Invalid UTF-8 sequence");

            unsigned int code_point = ((first_byte & 0x07) << 18) | ((second_byte & 0x3F) << 12) |
                                      ((third_byte & 0x3F) << 6) | (fourth_byte & 0x3F);

            code_point -= 0x10000;
            ensure_capacity(utf16_index + 2);
            ptr[utf16_index++] = 0xD800 | ((code_point >> 10) & 0x3FF);
            ptr[utf16_index++] = 0xDC00 | (code_point & 0x3FF);

            i += 4;
        }
        else
        {
            Log::fatal("Invalid UTF-8 start byte");
        }
    }

    len = utf16_index;
}

void String::convert_utf16le_to_utf16(const char *str, long len_in)
{
    ensure_capacity(len_in / 2);
    len = 0;

    for (long i = 0; i < len_in; i += 2)
    {
        String::udata_type code_unit =
            (static_cast<unsigned char>(str[i + 1]) << 8) | static_cast<unsigned char>(str[i]);

        ensure_capacity(len + 1);
        ptr[len++] = static_cast<String::data_type>(code_unit);

        if (code_unit >= 0xD800 && code_unit <= 0xDBFF)
        {
            if (i + 2 >= len_in)
                Log::fatal("Incomplete surrogate pair");

            String::udata_type low_surrogate =
                (static_cast<unsigned char>(str[i + 3]) << 8) | static_cast<unsigned char>(str[i + 2]);

            if (low_surrogate < 0xDC00 || low_surrogate > 0xDFFF)
                Log::fatal("Invalid low surrogate");

            ensure_capacity(len + 1);
            ptr[len++] = static_cast<String::data_type>(low_surrogate);
            i += 2;
        }
    }
}

void String::convert_utf16be_to_utf16(const char *str, long len_in)
{
    ensure_capacity(len_in / 2);
    len = 0;

    for (long i = 0; i < len_in; i += 2)
    {
        String::udata_type code_unit =
            (static_cast<unsigned char>(str[i]) << 8) | static_cast<unsigned char>(str[i + 1]);

        ensure_capacity(len + 1);
        ptr[len++] = static_cast<String::data_type>(code_unit);

        if (code_unit >= 0xD800 && code_unit <= 0xDBFF)
        {
            if (i + 2 >= len_in)
                Log::fatal("Incomplete surrogate pair");

            String::udata_type low_surrogate =
                (static_cast<unsigned char>(str[i + 2]) << 8) | static_cast<unsigned char>(str[i + 3]);

            if (low_surrogate < 0xDC00 || low_surrogate > 0xDFFF)
                Log::fatal("Invalid low surrogate");

            ensure_capacity(len + 1);
            ptr[len++] = static_cast<String::data_type>(low_surrogate);
            i += 2;
        }
    }
}

void String::convert_utf32le_to_utf16(const char *str, long len_in)
{
    ensure_capacity(len_in / 4 * 2);
    len = 0;

    for (long i = 0; i < len_in; i += 4)
    {
        unsigned int code_point = static_cast<unsigned char>(str[i]) | (static_cast<unsigned char>(str[i + 1]) << 8) |
                                  (static_cast<unsigned char>(str[i + 2]) << 16) |
                                  (static_cast<unsigned char>(str[i + 3]) << 24);

        if (code_point <= 0xFFFF)
        {
            ensure_capacity(len + 1);
            ptr[len++] = static_cast<String::data_type>(code_point);
        }
        else if (code_point <= 0x10FFFF)
        {
            code_point -= 0x10000;
            ensure_capacity(len + 2);
            ptr[len++] = 0xD800 | ((code_point >> 10) & 0x3FF);
            ptr[len++] = 0xDC00 | (code_point & 0x3FF);
        }
        else
        {
            Log::fatal("Invalid Unicode code point");
        }
    }
}

void String::convert_utf32be_to_utf16(const char *str, long len_in)
{
    ensure_capacity(len_in / 4 * 2);
    len = 0;

    for (long i = 0; i < len_in; i += 4)
    {
        unsigned int code_point =
            (static_cast<unsigned char>(str[i]) << 24) | (static_cast<unsigned char>(str[i + 1]) << 16) |
            (static_cast<unsigned char>(str[i + 2]) << 8) | static_cast<unsigned char>(str[i + 3]);

        if (code_point <= 0xFFFF)
        {
            ensure_capacity(len + 1);
            ptr[len++] = static_cast<String::data_type>(code_point);
        }
        else if (code_point <= 0x10FFFF)
        {
            code_point -= 0x10000;
            ensure_capacity(len + 2);
            ptr[len++] = 0xD800 | ((code_point >> 10) & 0x3FF);
            ptr[len++] = 0xDC00 | (code_point & 0x3FF);
        }
        else
        {
            Log::fatal("Invalid Unicode code point");
        }
    }
}

void String::ensure_capacity(String::size_type new__str_capacity)
{
    if (new__str_capacity > _str_capacity)
    {
        String::size_type new_cap = (_str_capacity == 0) ? 8 : _str_capacity * 2;
        if (new_cap < new__str_capacity)
        {
            new_cap = new__str_capacity;
        }
        String::data_type *new_ptr = new String::data_type[new_cap];
        if (ptr != nullptr && len > 0)
        {
            memcpy(new_ptr, ptr, len * sizeof(String::data_type));
        }
        delete[] ptr;
        ptr = new_ptr;
        _str_capacity = new_cap;
    }
}
} // namespace LunaVoxelEngine::Utils
