#include <platform/common_log.h>
#include <platform/log.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <utils/algorithm.h>
#include <utils/string.h>

namespace LunaVoxelEngine::Log
{
static char buffer[BUFFER_SIZE];
static size_t buffer_pos = 0;

inline void flush() noexcept
{
    if (buffer_pos > 0)
    {
        syscall(SYS_write, 1, buffer, buffer_pos);
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
    buffer[buffer_pos++] = c;
}

void fatal(const Utils::String &fmt, ...) noexcept
{
    va_list args;
    va_start(args, fmt);
    write_str("LUNAVOXEL - FATAL: ", 20);
    auto str = fmt.throw_away();
    print_generic(str.c_str(), args);
    write_char('\n');
    va_end(args);
    syscall(SYS_exit, 1);
}
} // namespace LunaVoxelEngine::Log
