#include <platform/Log.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
namespace LunaVoxalEngine::Log
{
void trace(const char *fmt, ...) noexcept
{
    va_list args;
    va_start(args, fmt);
    printf("LUNAVOXAL - TRACE: ");
    vprintf(fmt, args);
    va_end(args);
}
void debug(const char *fmt, ...) noexcept
{
#ifdef DEBUG
    va_list args;
    va_start(args, fmt);
    printf("LUNAVOXAL - TRACE: ");
    vprintf(fmt, args);
    va_end(args);
#endif
}
void info(const char *fmt, ...) noexcept
{
    va_list args;
    va_start(args, fmt);
    printf("LUNAVOXAL - INFO: ");
    vprintf(fmt, args);
    putchar('\n');
    va_end(args);
}
void warn(const char *fmt, ...) noexcept
{
    va_list args;
    va_start(args, fmt);
    printf("LUNAVOXAL - WARN: ");
    vprintf(fmt, args);
    putchar('\n');
    va_end(args);
}
void error(const char *fmt, ...) noexcept
{
    va_list args;
    va_start(args, fmt);
    printf("LUNAVOXAL - ERROR: ");
    putchar('\n');
    vprintf(fmt, args);
    va_end(args);
}
void fatal(const char *fmt, ...) noexcept
{
    va_list args;
    va_start(args, fmt);
    printf("LUNAVOXAL - FATAL: ");
    vprintf(fmt, args);
    va_end(args);
    exit(1);
}
} // namespace LunaVoxalEngine::Log
