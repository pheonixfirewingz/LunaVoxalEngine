#include <platform/main.h>
#include <utils/new.h>

namespace LunaVoxalEngine::Platform
{
bool Runtime::Init(Utils::Vector<Utils::String> args) noexcept
{
    window = new Window(1280, 720, "LunaVoxalEngine");
    window->show();
    device = new Renderer::Device(true);
    swap_chain = new Renderer::SwapChain(device, window->getVulkanLink());
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