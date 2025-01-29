#ifndef VK_QUEUE_H
#define VK_QUEUE_H
#include <renderer/vulkan/ivulkan.h>

namespace LunaVoxelEngine::Renderer
{
    class [[nodiscard]] Queue final
    {
    public:
        Queue(unsigned long family_index)
        {
            vkGetDeviceQueue(volkGetLoadedDevice(), family_index, 0, &queue);
        }

        VkResult submit(VkSubmitInfo &submitInfo, VkFence fence = VK_NULL_HANDLE)
        {
            return vkQueueSubmit(queue, 1, &submitInfo, fence);
        }
        
        VkResult submit2(VkSubmitInfo2 &submitInfo, VkFence fence = VK_NULL_HANDLE)
        {
            return vkQueueSubmit2(queue, 1, &submitInfo, fence);
        }

        VkResult present(VkPresentInfoKHR &presentInfo)
        {
            return vkQueuePresentKHR(queue, &presentInfo);
        }


        void waitIdle()
        {
            vkQueueWaitIdle(queue);
        }
    private:
        VkQueue queue;
    };
}
#endif