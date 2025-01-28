#ifndef VK_SWAPCHAIN_H
#define VK_SWAPCHAIN_H
#include <renderer/vulkan/device.h>
namespace LunaVoxalEngine::Renderer
{
/**
 * @class SwapChain
 * @brief Swap Chain class
 * @details This class encapsulates a Vulkan swap chain with the window surface
 */
class SwapChain final
{
   public:
    /**
     * @brief Constructor
     * @details Creates a new swap chain with the given device and native window
     * @param[in] device The Vulkan device
     * @param[in] native_window The native window
     */
    SwapChain(const Device *device, void *native_window);
    /**
     * @brief Resize the swap chain
     * @details Resizes the swap chain with the new device
     * @param[in] device The new Vulkan device
     */
    void resize(const Device *device);
    /**
     * @brief Destructor
     * @details Destroys the swap chain
     */
    ~SwapChain();

   private:
    VkSurfaceKHR surface; ///< The window surface
    VkSwapchainKHR swap_chain = VK_NULL_HANDLE; ///< The swap chain
};
} // namespace LunaVoxalEngine::Renderer
#endif
