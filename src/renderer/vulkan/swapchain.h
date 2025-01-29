// swapchain.h additions
#ifndef VK_SWAPCHAIN_H
#define VK_SWAPCHAIN_H
#include <renderer/vulkan/device.h>
#include <renderer/vulkan/queue.h>
#include <platform/window.h>
#include <utils/vector.h>

namespace LunaVoxelEngine
{
namespace Renderer
{
class [[nodiscard]] SwapChain final
{
  public:
    /*@brief Constructor *@details Creates a new swap chain with the given device and native window
     *@param[in] device The Vulkan device *@param[in] native_window The native window
     */
    SwapChain(const Device *device, Platform::NativeWindow native_window);
    void resize(const Device *device);
    ~SwapChain();

    VkSwapchainKHR get() const { return swap_chain; }
    VkSurfaceKHR get_surface() const { return surface; }

    /**
     * @brief Acquire the next image from the swapchain
     * @param[in] semaphore Semaphore to signal when image is acquired
     * @param[in] fence Fence to signal when image is acquired (optional)
     * @param[out] imageIndex Index of the acquired image
     * @return VkResult indicating success or failure
     */
    VkResult acquireNextImage(VkSemaphore semaphore, VkFence fence, uint32_t* imageIndex);

    /**
     * @brief Present an image to the swapchain
     * @param[in] queue Queue to submit presentation to
     * @param[in] imageIndex Index of image to present
     * @param[in] waitSemaphore Semaphore to wait on before presenting
     * @return VkResult indicating success or failure
     */
    VkResult present(Queue* queue, uint32_t imageIndex, VkSemaphore waitSemaphore);

    /**
     * @brief Get the swapchain image format
     * @return Format of the swapchain images
     */
    VkFormat getImageFormat() const { return image_format.format; }

    /**
     * @brief Get the swapchain image color space
     * @return Color space of the swapchain images
     */
    VkColorSpaceKHR getColorSpace() const { return image_format.colorSpace; }

    /**
     * @brief Get the swapchain extent
     * @return Extent of the swapchain images
     */
    VkExtent2D getExtent() const { return extent; }

    /**
     * @brief Get the swapchain image views
     * @return Vector of image views
     */
    Utils::Vector<VkImageView> getImageViews() const { return image_views; }

    /**
     * @brief Get the swapchain images
     * @return Vector of images
     */
    Utils::Vector<VkImage> getImages() const { return images; }

    /**
     * @brief Get the swapchain image count
     * @return Number of images in the swapchain
     */
    uint32_t getImageCount() const { return images.size(); }

   private:
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkSwapchainKHR swap_chain = VK_NULL_HANDLE;
    Utils::Vector<VkImage> images;
    Utils::Vector<VkImageView> image_views;
    Utils::Vector<VkFramebuffer> frame_buffers;
    VkSurfaceFormatKHR image_format;
    VkExtent2D extent;
    VkPresentModeKHR present_mode;
};
} // namespace Renderer
} // namespace LunaVoxelEngine
#endif