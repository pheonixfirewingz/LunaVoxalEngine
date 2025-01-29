#ifndef VK_IMAGE_H
#define VK_IMAGE_H

#include <renderer/vulkan/cmd_buffer.h>
#include <renderer/vulkan/ivulkan.h>


namespace LunaVoxelEngine::Renderer
{
class Image final
{
  public:
    Image();
    void *map();
    void unmap();
    /**
     * @brief Transitions the image layout.
     *
     * @param cmdBuffer The command buffer to record the transition commands.
     * @param oldLayout The current layout of the image.
     * @param newLayout The desired layout of the image.
     * @param aspectMask The aspect mask for the image (e.g., color, depth).
     */
    void transitionLayout(CommandBuffer cmdBuffer, VkImageLayout oldLayout, VkImageLayout newLayout,
                          VkImageAspectFlags aspectMask);
    /**
     * @brief Gets the Vulkan image handle.
     * @return The VkImage handle.
     */
    VkImage getImage() const
    {
        return image_;
    }
    /**
     * @brief Gets the Vulkan image view handle.
     * @return The VkImageView handle.
     */
    VkImageView getImageView() const
    {
        return imageView_;
    }
    /**
     * @brief Destructor for Image.
     *
     * Automatically destroys the image, image view, and releases resources.
     */
    ~Image();
    // Prevent copying
    Image(const Image &) = delete;
    Image &operator=(const Image &) = delete;
    // Allow moving
    Image(Image &&other) noexcept;
    Image &operator=(Image &&other) noexcept;

  private:
    VkImage image_ = VK_NULL_HANDLE;         ///< The Vulkan image handle
    VkImageView imageView_ = VK_NULL_HANDLE; ///< The Vulkan image view handle
    VkFormat format_ = VK_FORMAT_UNDEFINED;  ///< Format of the image
    uint32_t width_ = 0;                     ///< Width of the image
    uint32_t height_ = 0;                    ///< Height of the image
};
} // namespace LunaVoxelEngine::Renderer
#endif
