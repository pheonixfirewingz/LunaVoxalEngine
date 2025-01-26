#include <platform/common_log.h>
#include <platform/log.h>
#include <utils/string.hpp>
namespace LunaVoxalEngine::Log
{

void convert_and_write_string(char* str) {
    write_str(str, strlen(str));
}

void convert_and_write_char(char value) {
    write_char(value);
}

void int_to_string(int num, char* str) {
    int is_negative = 0;
    int i = 0;

    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    do {
        str[i++] = (num % 10) + '0';
        num /= 10;
    } while (num > 0);

    if (is_negative) {
        str[i++] = '-';
    }

    str[i] = '\0';
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
}

void float_to_string(float num, char* str) {
    int int_part = (int) num;
    float fractional_part = num - int_part;
    int i = 0;
    if (int_part == 0) {
        str[i++] = '0';
    } else {
        if (int_part < 0) {
            str[i++] = '-';
            int_part = -int_part;
        }
        int int_str[20], int_len = 0;
        while (int_part > 0) {
            int_str[int_len++] = int_part % 10;
            int_part /= 10;
        }
        for (int j = int_len - 1; j >= 0; j--) {
            str[i++] = int_str[j] + '0';
        }
    }
    str[i++] = '.';
    fractional_part *= 100;
    int frac_part = (int) fractional_part;
    str[i++] = (frac_part / 10) + '0';
    str[i++] = (frac_part % 10) + '0';

    str[i] = '\0';
}

void convert_and_write_int(int value) {
    unsigned char buffer[1024*4];
    int_to_string(value, (char*)buffer);
    for (int i = 0; buffer[i] != '\0'; i++) {
        write_char(buffer[i]);
    }
}

void convert_and_write_float(double value) {
     unsigned char buffer[1024*4];
    float_to_string(value, (char*)buffer);
    for (int i = 0; buffer[i] != '\0'; i++) {
        write_char(buffer[i]);
    }
}

void convert_and_write_hex(int value) {
    // Convert and write the hexadecimal value (implement this yourself)
    // Example: Use a custom function to convert the integer to a hex string and then call write_str
    write_str("(Hex value TODO)", 2);
}

void print_generic(const char* format, va_list args) {
    size_t i = 0;
    while (format[i] != '\0') {
        if (format[i] == '%' && format[i + 1] != '\0') {
            i++;
            switch (format[i]) {
                case 'd': {
                    int num = va_arg(args, int);
                    convert_and_write_int(num);
                    break;
                }
                case 's': {
                    char* str = va_arg(args, char*);
                    convert_and_write_string(str);
                    break;
                }
                case 'c': {
                    char ch = (char) va_arg(args, int);
                    convert_and_write_char(ch);
                    break;
                }
                case 'f': {
                    double num = va_arg(args, double);
                    convert_and_write_float(num);
                    break;
                }
                case 'x': {
                    int num = va_arg(args, int);
                    convert_and_write_hex(num);
                    break;
                }
                case '%': {
                    write_char('%');
                    break;
                }
                default: {
                    write_char('%');
                    write_char(format[i]);
                    break;
                }
            }
        } else {
            write_char(format[i]);
        }
        i++;
    }
}


void trace(const Utils::String &fmt, ...) noexcept
{
    va_list args;
    va_start(args, fmt);
    write_str("LUNAVOXAL - TRACE: ", 20);
    auto str = fmt.to_std_string();
    print_generic(str.c_str(), args);
    write_char('\n');
    va_end(args);
}
void debug(const Utils::String &fmt, ...) noexcept
{
#ifdef DEBUG
    va_list args;
    va_start(args, fmt);
    write_str("LUNAVOXAL - DEBUG: ", 20);
    auto str = fmt.to_std_string();
    print_generic(str.c_str(), args);
    write_char('\n');
    va_end(args);
#endif
}
void info(const Utils::String &fmt, ...) noexcept
{
    va_list args;
    va_start(args, fmt);
    write_str("LUNAVOXAL - INFO: ", 20);
    auto str = fmt.to_std_string();
    print_generic(str.c_str(), args);
    write_char('\n');
    va_end(args);
}
void warn(const Utils::String &fmt, ...) noexcept
{
    va_list args;
    va_start(args, fmt);
    write_str("LUNAVOXAL - WARN: ", 20);
    auto str = fmt.to_std_string();
    print_generic(str.c_str(), args);
    write_char('\n');
    va_end(args);
}
void error(const Utils::String &fmt, ...) noexcept
{
    va_list args;
    va_start(args, fmt);
    write_str("LUNAVOXAL - ERROR: ", 20);
    auto str = fmt.to_std_string();
    print_generic(str.c_str(), args);
    write_char('\n');
    va_end(args);
}
}