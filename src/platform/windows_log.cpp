#include <platform/log.h>
#include <platform/common_log.h>
#include <utils/string.hpp>
#define WINAPI __stdcall
extern "C" {
    int WINAPI WriteConsole(void* hConsoleOutput, const void* lpBuffer, unsigned long nNumberOfCharsToWrite, unsigned long* lpNumberOfCharsWritten, void* lpReserved);
    void WINAPI ExitProcess(unsigned int uExitCode);
}

namespace LunaVoxalEngine::Log
{
static char buffer[BUFFER_SIZE];
static size_t buffer_pos = 0;
inline void flush() noexcept {
    if (buffer_pos > 0) {
        WriteConsole(nullptr, buffer, static_cast<unsigned long>(buffer_pos), nullptr, nullptr);
        buffer_pos = 0;
    }
}

void write_str(const char* str, size_t len) noexcept {
    if (len + buffer_pos >= BUFFER_SIZE) {
        flush();
    }
    size_t remaining_space = BUFFER_SIZE - buffer_pos;
    size_t copy_len = (len < remaining_space) ? len : remaining_space;

    memcpy(buffer + buffer_pos, str, copy_len);
    buffer_pos += copy_len;
    if (copy_len < len) {
        flush();
        write_str(str + copy_len, len - copy_len);
    }
}

void write_char(const char c) noexcept {
    if (buffer_pos >= BUFFER_SIZE || c == '\n') {
        flush();
    }
    buffer[buffer_pos++] = c;
}

void fatal(const Utils::String &fmt, ...) noexcept {
    va_list args;
    va_start(args, fmt);
    write_str("LUNAVOXAL - FATAL: ", 20);
    auto str = fmt.to_std_string();
    print_generic(str.c_str(), args);
    write_char('\n');
    va_end(args);

    // Call the stubbed ExitProcess function to terminate the program
    ExitProcess(1);
}
} // namespace LunaVoxalEngine::Log
