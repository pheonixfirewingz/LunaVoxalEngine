#include <platform/main.h>
#include <stdlib.h>

namespace LunaVoxalEngine::Platform
{
bool Runtime::Init(std::vector<std::string> args) noexcept
{
    std::atexit(Runtime::_Shutdown);
    window = new Window::Window(1280, 720, "LunaVoxalEngine");
    window->show();
    return false;
}

bool Runtime::IsRunning() const noexcept
{
    return !window->shouldClose();
}

void Runtime::Update() noexcept
{  
    window->pollEvents();
}

void Runtime::Shutdown() noexcept
{  
    delete window;
}
} // namespace LunaVoxalEngine::Platform