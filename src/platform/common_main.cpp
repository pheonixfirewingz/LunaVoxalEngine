#include <platform/main.h>
#include <stdlib.h>

namespace LunaVoxalEngine::Platform
{
bool Runtime::Init(std::vector<std::string> args) noexcept
{
    std::atexit(Runtime::_Shutdown);
    return false;
}

bool Runtime::IsRunning() const noexcept
{
    return false;
}

void Runtime::Update() noexcept
{   
}

void Runtime::Shutdown() noexcept
{   
}
} // namespace LunaVoxalEngine::Platform