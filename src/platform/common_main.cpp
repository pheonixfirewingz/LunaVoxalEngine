#include <platform/main.h>

namespace LunaVoxalEngine::Platform
{
bool Runtime::Init(std::vector<Utils::String> args) noexcept
{
    std::atexit(Runtime::_Shutdown);
    window = new Window::Window(1280, 720, "LunaVoxalEngine");
    window->show();
    device = new Renderer::Device(true);
    swap_chain = new Renderer::SwapChain(device, window->getvulkanLink());
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
    delete swap_chain;
    delete device;
    delete window;
}
} // namespace LunaVoxalEngine::Platform