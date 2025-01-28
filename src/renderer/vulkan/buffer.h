#ifndef VK_BUFFER_H
#define VK_BUFFER_H
#include <renderer/vulkan/ivulkan.h>
namespace LunaVoxalEngine::Renderer
{
class Buffer final {
public:
    Buffer();
    /**
     * @brief Maps the buffer for writing.
     * 
     * @return A pointer to the mapped memory.
     * @warning the function will crash the engine if the buffer is not mapped or valid.
     */
    void *map();
    /**
     * @brief Unmaps the buffer.
     * 
     * @warning the function will crash the engine if the buffer is valid.
     */
    void unmap();
    /**
     * @brief Destructor for Buffer.
     *
     * Automatically destroys the buffer and releases its resources.
     */
    ~Buffer();
    ///< Prevent copying
    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    ///< Allow moving
    Buffer(Buffer&& other) noexcept;
    Buffer& operator=(Buffer&& other) noexcept;
    private:
    friend class CommandBuffer;
    VkBuffer buffer = VK_NULL_HANDLE;     ///< The Vulkan buffer handle
    VkDeviceSize size = 0;               ///< Size of the buffer in bytes
    VkBufferUsageFlags usage = 0;        ///< Usage flags for the buffer
};
} // namespace LunaVoxalEngine::Renderer
#endif