#ifndef COMMON_MAIN_H
#define COMMON_MAIN_H
#include <utils/string.h>
#include <utils/vector.h>
#include <platform/thread.h>
#include <platform/window.h>
#include <renderer/vulkan/device.h>
#include <renderer/vulkan/swapchain.h>
#include <renderer/vulkan/cmd_buffer.h>
#include <renderer/vulkan/pipeline.h>
#include <renderer/vulkan/queue.h>
namespace LunaVoxalEngine::Platform
{
class Runtime final
{
  public:
    bool Init(Utils::Vector<Utils::String> args) noexcept;
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
    Window *window;
    Renderer::Queue *queue;
    Renderer::Pipeline* pipeline;
    Renderer::Device *device;
    Renderer::SwapChain *swap_chain;
    Renderer::CommandBuffer *command_buffer;
};
} // namespace LunaVoxalEngine::Platform
#endif