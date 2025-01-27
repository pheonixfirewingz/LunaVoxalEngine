#ifndef COMMON_MAIN_H
#define COMMON_MAIN_H
#include <utils/string.hpp>
#include <platform/thread.h>
#include <platform/window.h>
#include <renderer/vulkan/device.h>
#include <renderer/vulkan/swapchain.h>
#include <vector>
namespace LunaVoxalEngine::Platform
{
class Runtime final
{
  public:
    bool Init(std::vector<Utils::String> args) noexcept;
    bool IsRunning() const noexcept;
    void Update() noexcept;
    void Shutdown() noexcept;
    static Runtime *Get() noexcept
    {
        static Runtime *runtime;
        [[unlikely]] if (!runtime)
        {
            runtime = new Runtime();
        }
        return runtime;
    }
    static void _Shutdown() noexcept
    {
        Runtime::Get()->Shutdown();
    }

  private:
    Window::Window *window;
    Renderer::Device *device;
    Renderer::SwapChain *swap_chain;
};
} // namespace LunaVoxalEngine::Platform
#endif