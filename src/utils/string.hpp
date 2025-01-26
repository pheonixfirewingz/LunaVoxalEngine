#ifndef UTILS_STRING_HPP
#define UTILS_STRING_HPP
#include <cstring>
#include <stdexcept>
namespace LunaVoxalEngine::Utils
{

class String
{
  public:
    // Constructors
    String() = default;

    String(const short *str, long len_in = -1)
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
        std::copy(str, str + len, ptr);
    }

    String(const char *str, long len_in = -1)
    {
        Encoding encoding = detect_encoding(str, len_in);
        convert_to_utf16(str, len_in, encoding);
    }

    String(const String &other)
        : len(other.len)
        , _str_capacity(other._str_capacity)
    {
        if (len > 0)
        {
            ptr = new short[_str_capacity];
            std::copy(other.ptr, other.ptr + len, ptr);
        }
    }

    String(String &&other) noexcept
        : ptr(other.ptr)
        , len(other.len)
        , _str_capacity(other._str_capacity)
    {
        other.ptr = nullptr;
        other.len = 0;
        other._str_capacity = 0;
    }

    ~String()
    {
        delete[] ptr;
    }

    String &operator=(const String &other)
    {
        if (this != &other)
        {
            delete[] ptr;
            len = other.len;
            _str_capacity = other._str_capacity;
            ptr = new short[_str_capacity];
            std::copy(other.ptr, other.ptr + len, ptr);
        }
        return *this;
    }

    String &operator=(String &&other) noexcept
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

    String &operator=(const short *str)
    {
        while (str[len] != 0)
        {
            ++len;
        }
        ensure_capacity(len);
        std::copy(str, str + len, ptr);
        return *this;
    }

    String &operator+(const char *str)
    {
        append(str);
        return *this;
    }

    String &operator+(const short *str)
    {
        append(str);
        return *this;
    }

    String &operator+(String &str)
    {
        append(str);
        return *this;
    }

    String &operator+(const String &str)
    {
        append(str);
        return *this;
    }

    friend String operator+(const char* lhs, const String& rhs) {
        return String(lhs) + rhs;
    }


    friend String operator+(const short* lhs, const String& rhs) {
        return String(lhs) + rhs;
    }

    unsigned long size() const noexcept
    {
        return len;
    }
    unsigned long length() const noexcept
    {
        return len;
    }
    bool empty() const noexcept
    {
        return len == 0;
    }

    unsigned long capacity() const noexcept
    {
        return _str_capacity;
    }

    void reserve(unsigned long new__str_capacity)
    {
        ensure_capacity(new__str_capacity);
    }

    short at(unsigned long index) const
    {
        if (index >= len)
        {
            throw std::out_of_range("Index out of range");
        }
        return ptr[index];
    }

    short &at(unsigned long index)
    {
        if (index >= len)
        {
            throw std::out_of_range("Index out of range");
        }
        return ptr[index];
    }

    short operator[](unsigned long index) const
    {
        return at(index);
    }

    short &operator[](unsigned long index)
    {
        return at(index);
    }

    short *data() noexcept
    {
        return ptr;
    }

    short front() const
    {
        return ptr[0];
    }

    short back() const
    {
        return ptr[len - 1];
    }

    short *raw_data() noexcept
    {
        return ptr;
    }
    const short *raw_data() const noexcept
    {
        return ptr;
    }

    void push_back(short ch)
    {
        ensure_capacity(len + 1);
        ptr[len++] = ch;
    }

    void pop_back() noexcept
    {
        if (len > 0)
        {
            --len;
        }
    }

    void shrink_to_fit() noexcept
    {
        _str_capacity = len;
        short *new_ptr = new short[_str_capacity];
        std::copy(ptr, ptr + len, new_ptr);
        delete[] ptr;
        ptr = new_ptr;
    }

    void resize(unsigned long new_size)
    {
        ensure_capacity(new_size);
        len = new_size;
    }

    String &append(const char *str)
    {
        auto encoding = detect_encoding(str, -1);
        convert_to_utf16(str, -1, encoding);
        return *this;
    }

    String &operator+=(const char *str)
    {
        return append(str);
    }

    String &append(const String &other)
    {
        ensure_capacity(len + other.len);
        std::copy(other.ptr, other.ptr + other.len, ptr + len);
        len += other.len;
        return *this;
    }

    String &operator+=(const String &other)
    {
        return append(other);
    }

    String &append(short ch)
    {
        ensure_capacity(len + 1);
        ptr[len++] = ch;
        return *this;
    }

    String &operator+=(short ch)
    {
        return append(ch);
    }

    String substr(unsigned long start, unsigned long end) const
    {
        String substr;
        substr.reserve(end - start);
        for (unsigned long i = start; i < end; ++i)
        {
            substr.push_back(ptr[i]);
        }
        return substr;
    }

    bool operator==(const String &other) const noexcept
    {
        if (len != other.len)
        {
            return false;
        }
        return std::equal(ptr, ptr + len, other.ptr);
    }

    bool operator!=(const String &other) const noexcept
    {
        return !(*this == other);
    }

    const std::string to_std_string() const noexcept
    {
        if (!ptr)
            return "";

        std::string utf8_result;
        
        unsigned long i = 0;

        while (i < len)
        {
            unsigned short code_unit = ptr[i++];

            if (code_unit < 0x80)
            {
                // 1-byte UTF-8
                utf8_result += static_cast<char>(code_unit);
            }
            else if (code_unit < 0x800)
            {
                // 2-byte UTF-8
                utf8_result += static_cast<char>(0xC0 | (code_unit >> 6));
                utf8_result += static_cast<char>(0x80 | (code_unit & 0x3F));
            }
            else if (code_unit >= 0xD800 && code_unit <= 0xDBFF)
            {
                // Surrogate pair: high surrogate
                unsigned short high_surrogate = code_unit;
                unsigned short low_surrogate = ptr[i];

                if (low_surrogate >= 0xDC00 && low_surrogate <= 0xDFFF)
                {
                    // Valid surrogate pair
                    i++; // Consume the low surrogate
                    unsigned int code_point = 0x10000 + ((high_surrogate - 0xD800) << 10) + (low_surrogate - 0xDC00);

                    // Encode the code point as 4-byte UTF-8
                    utf8_result += static_cast<char>(0xF0 | (code_point >> 18));
                    utf8_result += static_cast<char>(0x80 | ((code_point >> 12) & 0x3F));
                    utf8_result += static_cast<char>(0x80 | ((code_point >> 6) & 0x3F));
                    utf8_result += static_cast<char>(0x80 | (code_point & 0x3F));
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
                utf8_result += static_cast<char>(0xE0 | (code_unit >> 12));
                utf8_result += static_cast<char>(0x80 | ((code_unit >> 6) & 0x3F));
                utf8_result += static_cast<char>(0x80 | (code_unit & 0x3F));
            }
        }

        return utf8_result;
    }

    bool start_with(const String &other) const noexcept
    {
        if (len < other.len)
        {
            return false;
        }
        return std::equal(ptr, ptr + other.len, other.ptr);
    }

    bool end_with(const String &other) const noexcept
    {
        if (len < other.len)
        {
            return false;
        }
        return std::equal(ptr + len - other.len, ptr + len, other.ptr);
    }

    void clear() noexcept
    {
        delete[] ptr;
        ptr = nullptr;
        len = 0;
        _str_capacity = 0;
    }

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

    long get_utf16_length(const char *str, Encoding encoding)
    {
        if (!str)
            return 0;

        // Skip BOM
        if (encoding == Encoding::UTF8 && static_cast<unsigned char>(str[0]) == 0xEF &&
            static_cast<unsigned char>(str[1]) == 0xBB && static_cast<unsigned char>(str[2]) == 0xBF)
        {
            str += 3;
        }
        else if ((encoding == Encoding::UTF16_BE || encoding == Encoding::UTF16_LE) &&
                 static_cast<unsigned char>(str[0]) == 0xFE && static_cast<unsigned char>(str[1]) == 0xFF)
        {
            str += 2;
        }
        else if ((encoding == Encoding::UTF32_BE || encoding == Encoding::UTF32_LE) &&
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
        case Encoding::ASCII:
            utf16_len = std::strlen(str);
            break;

        case Encoding::UTF8: {
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

        case Encoding::UTF16_LE:
        case Encoding::UTF16_BE: {
            const unsigned short *ptr = reinterpret_cast<const unsigned short *>(str);
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

        case Encoding::UTF32_LE:
        case Encoding::UTF32_BE: {
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

    Encoding detect_encoding(const char *str, long len_in)
    {
        // Determine actual length if not provided
        if (len_in == -1)
        {
            len_in = std::strlen(str);
        }

        if (len_in < 2)
            return Encoding::ASCII;

        const unsigned char *bytes = reinterpret_cast<const unsigned char *>(str);

        // UTF-8 BOM check
        if (len_in >= 3 && bytes[0] == 0xEF && bytes[1] == 0xBB && bytes[2] == 0xBF)
            return Encoding::UTF8;

        // UTF-16 BOM check
        if (len_in >= 2)
        {
            if (bytes[0] == 0xFE && bytes[1] == 0xFF)
                return Encoding::UTF16_BE;
            if (bytes[0] == 0xFF && bytes[1] == 0xFE)
                return Encoding::UTF16_LE;
        }

        // UTF-32 BOM check
        if (len_in >= 4)
        {
            if (bytes[0] == 0x00 && bytes[1] == 0x00 && bytes[2] == 0xFE && bytes[3] == 0xFF)
                return Encoding::UTF32_BE;
            if (bytes[0] == 0xFF && bytes[1] == 0xFE && bytes[2] == 0x00 && bytes[3] == 0x00)
                return Encoding::UTF32_LE;
        }

        return detect_utf8_heuristic(str, len_in);
    }

    Encoding detect_utf8_heuristic(const char *str, long len_in)
    {
        size_t utf8_sequence_count = 0;
        size_t expected_continuation_bytes = 0;

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
                    return Encoding::ASCII;

                utf8_sequence_count++;
            }
            else
            {
                if ((byte & 0xC0) != 0x80)
                    return Encoding::ASCII;
                expected_continuation_bytes--;
            }
        }

        return (utf8_sequence_count > 0) ? Encoding::UTF8 : Encoding::ASCII;
    }

    void convert_to_utf16(const char *str, long len_in, Encoding encoding)
    {
        if (len_in == -1)
        {
            len_in = std::strlen(str);
        }

        ensure_capacity(len_in);
        len = 0;

        switch (encoding)
        {
        case Encoding::ASCII:
            for (long i = 0; i < len_in; ++i)
            {
                ptr[len++] = static_cast<short>(str[i]);
            }
            break;

        case Encoding::UTF8:
            convert_utf8_to_utf16(str, len_in);
            break;

        case Encoding::UTF16_LE:
            convert_utf16le_to_utf16(str, len_in);
            break;

        case Encoding::UTF16_BE:
            convert_utf16be_to_utf16(str, len_in);
            break;

        case Encoding::UTF32_LE:
            convert_utf32le_to_utf16(str, len_in);
            break;

        case Encoding::UTF32_BE:
            convert_utf32be_to_utf16(str, len_in);
            break;
        }
    }

    void convert_utf8_to_utf16(const char *str, long len_in)
    {
        unsigned long utf16_index = 0;
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
                    throw std::runtime_error("Incomplete UTF-8 sequence");

                unsigned char second_byte = static_cast<unsigned char>(str[i + 1]);
                if ((second_byte & 0xC0) != 0x80)
                    throw std::runtime_error("Invalid UTF-8 sequence");

                unsigned int code_point = ((first_byte & 0x1F) << 6) | (second_byte & 0x3F);
                ensure_capacity(utf16_index + 1);
                ptr[utf16_index++] = static_cast<short>(code_point);
                i += 2;
            }

            else if ((first_byte & 0xF0) == 0xE0)
            {
                if (i + 2 >= len_in)
                    throw std::runtime_error("Incomplete UTF-8 sequence");

                unsigned char second_byte = static_cast<unsigned char>(str[i + 1]);
                unsigned char third_byte = static_cast<unsigned char>(str[i + 2]);

                if ((second_byte & 0xC0) != 0x80 || (third_byte & 0xC0) != 0x80)
                    throw std::runtime_error("Invalid UTF-8 sequence");

                unsigned int code_point =
                    ((first_byte & 0x0F) << 12) | ((second_byte & 0x3F) << 6) | (third_byte & 0x3F);
                ensure_capacity(utf16_index + 1);
                ptr[utf16_index++] = static_cast<short>(code_point);
                i += 3;
            }

            else if ((first_byte & 0xF8) == 0xF0)
            {
                if (i + 3 >= len_in)
                    throw std::runtime_error("Incomplete UTF-8 sequence");

                unsigned char second_byte = static_cast<unsigned char>(str[i + 1]);
                unsigned char third_byte = static_cast<unsigned char>(str[i + 2]);
                unsigned char fourth_byte = static_cast<unsigned char>(str[i + 3]);

                if ((second_byte & 0xC0) != 0x80 || (third_byte & 0xC0) != 0x80 || (fourth_byte & 0xC0) != 0x80)
                    throw std::runtime_error("Invalid UTF-8 sequence");

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
                throw std::runtime_error("Invalid UTF-8 start byte");
            }
        }

        len = utf16_index;
    }

    void convert_utf16le_to_utf16(const char *str, long len_in)
    {
        ensure_capacity(len_in / 2);
        len = 0;

        for (long i = 0; i < len_in; i += 2)
        {
            unsigned short code_unit =
                (static_cast<unsigned char>(str[i + 1]) << 8) | static_cast<unsigned char>(str[i]);

            ensure_capacity(len + 1);
            ptr[len++] = static_cast<short>(code_unit);

            if (code_unit >= 0xD800 && code_unit <= 0xDBFF)
            {
                if (i + 2 >= len_in)
                    throw std::runtime_error("Incomplete surrogate pair");

                unsigned short low_surrogate =
                    (static_cast<unsigned char>(str[i + 3]) << 8) | static_cast<unsigned char>(str[i + 2]);

                if (low_surrogate < 0xDC00 || low_surrogate > 0xDFFF)
                    throw std::runtime_error("Invalid low surrogate");

                ensure_capacity(len + 1);
                ptr[len++] = static_cast<short>(low_surrogate);
                i += 2;
            }
        }
    }

    void convert_utf16be_to_utf16(const char *str, long len_in)
    {
        ensure_capacity(len_in / 2);
        len = 0;

        for (long i = 0; i < len_in; i += 2)
        {
            unsigned short code_unit =
                (static_cast<unsigned char>(str[i]) << 8) | static_cast<unsigned char>(str[i + 1]);

            ensure_capacity(len + 1);
            ptr[len++] = static_cast<short>(code_unit);

            if (code_unit >= 0xD800 && code_unit <= 0xDBFF)
            {
                if (i + 2 >= len_in)
                    throw std::runtime_error("Incomplete surrogate pair");

                unsigned short low_surrogate =
                    (static_cast<unsigned char>(str[i + 2]) << 8) | static_cast<unsigned char>(str[i + 3]);

                if (low_surrogate < 0xDC00 || low_surrogate > 0xDFFF)
                    throw std::runtime_error("Invalid low surrogate");

                ensure_capacity(len + 1);
                ptr[len++] = static_cast<short>(low_surrogate);
                i += 2;
            }
        }
    }

    void convert_utf32le_to_utf16(const char *str, long len_in)
    {
        ensure_capacity(len_in / 4 * 2);
        len = 0;

        for (long i = 0; i < len_in; i += 4)
        {
            unsigned int code_point =
                static_cast<unsigned char>(str[i]) | (static_cast<unsigned char>(str[i + 1]) << 8) |
                (static_cast<unsigned char>(str[i + 2]) << 16) | (static_cast<unsigned char>(str[i + 3]) << 24);

            if (code_point <= 0xFFFF)
            {
                ensure_capacity(len + 1);
                ptr[len++] = static_cast<short>(code_point);
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
                throw std::runtime_error("Invalid Unicode code point");
            }
        }
    }

    void convert_utf32be_to_utf16(const char *str, long len_in)
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
                ptr[len++] = static_cast<short>(code_point);
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
                throw std::runtime_error("Invalid Unicode code point");
            }
        }
    }
    void ensure_capacity(unsigned long new__str_capacity)
    {
        if (new__str_capacity > _str_capacity)
        {
            unsigned long new_cap = (_str_capacity == 0) ? 8 : _str_capacity * 2;
            if (new_cap < new__str_capacity)
            {
                new_cap = new__str_capacity;
            }
            short *new_ptr = new short[new_cap];
            if (ptr != nullptr && len > 0)
            {
                std::copy(ptr, ptr + len, new_ptr);
            }
            delete[] ptr;
            ptr = new_ptr;
            _str_capacity = new_cap;
        }
    }
};
} // namespace LunaVoxalEngine::Utils
#endif
