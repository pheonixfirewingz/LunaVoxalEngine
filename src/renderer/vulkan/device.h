#ifndef VK_DEVICE_H
#define VK_DEVICE_H
#include <renderer/vulkan/ivulkan.h>

namespace LunaVoxelEngine::Renderer
{

/**
 * @class  Device
 * @brief  The vulkan device class.
 */
class Device final
{
  public:
    /**
     * @brief  Constructor.
     * @param  debug  If true, enable vulkan debug messages.
     */
    Device(const bool debug);

    /**
     * @brief  Destructor.
     */
    ~Device();

    /**
     * @brief  Get the vulkan physical device.
     * @return The vulkan physical device.
     */
    [[nodiscard]] VkPhysicalDevice get_physical_device() const
    {
        return physical_device_;
    }

    /**
     * @brief  Get the vulkan graphics queue.
     * @return The vulkan graphics queue.
     */
    [[nodiscard]] VkQueue get_graphics_queue() const
    {
        return graphics_queue_;
    }

    /**
     * @brief  Get the graphics family index.
     * @return The graphics family index.
     */
    [[nodiscard]] uint32_t get_graphics_family_index() const
    {
        return graphics_family_index_;
    }

  private:
    /** The vulkan debug utils messenger. */
    VkDebugUtilsMessengerEXT debug_messenger_ = VK_NULL_HANDLE;
    /** The physical vulkan device. */
    VkPhysicalDevice physical_device_;
    /** The vulkan graphics queue. */
    VkQueue graphics_queue_;
    /** The graphics family index. */
    uint32_t graphics_family_index_ = 0;
};
} // namespace LunaVoxelEngine::Renderer
#endif