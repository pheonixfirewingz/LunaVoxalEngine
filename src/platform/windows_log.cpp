#include <platform/common_log.h>
#include <platform/log.h>
#include <utils/string.h>
#include <utils/algorithm.h>
#include <Windows.h>

namespace LunaVoxelEngine::Log
{
static char buffer[BUFFER_SIZE];
static size_t buffer_pos = 0;
inline void flush() noexcept
{
    if (buffer_pos > 0)
    {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole != nullptr && hConsole != INVALID_HANDLE_VALUE)
        {
            WriteConsole(hConsole, buffer, static_cast<unsigned long>(buffer_pos), nullptr, nullptr);
        }
        buffer_pos = 0;
    }
}

void write_str(const char *str, size_t len) noexcept
{
    if (len + buffer_pos >= BUFFER_SIZE)
    {
        flush();
    }
    size_t remaining_space = BUFFER_SIZE - buffer_pos;
    size_t copy_len = (len < remaining_space) ? len : remaining_space;

    Utils::memcpy(buffer + buffer_pos, str, copy_len);
    buffer_pos += copy_len;
    if (copy_len < len)
    {
        flush();
        write_str(str + copy_len, len - copy_len);
    }
}

void write_char(const char c) noexcept
{
    if (buffer_pos >= BUFFER_SIZE || c == '\n')
    {
        flush();
    }
    [[likely]] if (buffer_pos < BUFFER_SIZE)
    {
        buffer[buffer_pos++] = c;
    }
}

void fatal(const Utils::String &fmt, ...) noexcept
{
    va_list args;
    auto str = fmt.throw_away();
    va_start(args, str.c_str());
    write_str("LUNAVOXEL - FATAL: ", 20);
    print_generic(str.c_str(), args);
    write_char('\n');
    va_end(args);
    ExitProcess(1);
}
} // namespace LunaVoxelEngine::Log
