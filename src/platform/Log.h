#ifndef LOG_H
#define LOG_H

namespace LunaVoxalEngine::Log
{
    void trace(const char *fmt, ...) noexcept;
    void debug(const char *fmt, ...) noexcept;
    void info(const char *fmt, ...) noexcept;
    void warn(const char *fmt, ...) noexcept;
    void error(const char *fmt, ...) noexcept;
} // namespace LunaVoxalEngine::Platform
#endif