#ifndef LOG_H
#define LOG_H
#include <utils/string.hpp>

namespace LunaVoxalEngine::Log
{
void trace(const Utils::String &fmt, ...) noexcept;
void debug(const Utils::String &fmt, ...) noexcept;
void info(const Utils::String &fmt, ...) noexcept;
void warn(const Utils::String &fmt, ...) noexcept;
void error(const Utils::String &fmt, ...) noexcept;
void fatal(const Utils::String &fmt, ...) noexcept;
} // namespace LunaVoxalEngine::Log
#endif