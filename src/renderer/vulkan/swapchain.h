#ifndef VK_SWAPCHAIN_H
#define VK_SWAPCHAIN_H
#include <renderer/vulkan/device.h>
namespace LunaVoxalEngine::Renderer
{
struct SwapChain
{
    VkSurfaceKHR surface;
    VkSwapchainKHR swap_chain = VK_NULL_HANDLE;
    SwapChain(const Device *device, void *native_window);
    void resize(const Device *device);
    ~SwapChain();
};
} // namespace LunaVoxalEngine::Renderer
#endif