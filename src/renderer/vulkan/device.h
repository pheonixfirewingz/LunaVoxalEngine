#ifndef VK_DEVICE_H
#define VK_DEVICE_H
#include <renderer/vulkan/ivulkan.h>
namespace LunaVoxalEngine::Renderer
{
struct Device
{
    VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;
    VkPhysicalDevice physical_device;
    VkQueue graphics_queue;
    uint32_t graphics_family_index = 0;
    Device(const bool debug);
    ~Device();
};
} // namespace LunaVoxalEngine::Renderer
#endif