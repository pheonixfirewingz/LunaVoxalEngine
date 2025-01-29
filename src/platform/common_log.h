#ifndef COMMON_LOG_H
#define COMMON_LOG_H
#include <utils/cdef.h>
#include <cstdarg>
constexpr auto BUFFER_SIZE = 1024;
namespace LunaVoxelEngine
{
namespace Log 
{
void write_str(const char *str, size_t len) noexcept;
void write_char(const char c) noexcept;
void print_generic(const char *format, va_list args);
} // namespace Log
} // namespace LunaVoxelEngine
#endif
