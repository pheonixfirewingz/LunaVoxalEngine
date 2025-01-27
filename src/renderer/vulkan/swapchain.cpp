#include <platform/log.h>

#include <renderer/vulkan/swapchain.h>

 VkSurfaceFormatKHR chooseSwapSurfaceFormat(const VkSurfaceFormatKHR* availableFormats, uint32_t formatCount) {
        uint32_t i = 0;
        while (i < formatCount) {
            VkSurfaceFormatKHR availableFormat = availableFormats[i];
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }
        return availableFormats[0];
    }

    VkPresentModeKHR chooseSwapPresentMode(const VkPresentModeKHR* availablePresentModes, uint32_t presentModeCount) {
        uint32_t i = 0;
        while (i < presentModeCount) {
            VkPresentModeKHR availablePresentMode = availablePresentModes[i];
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

static bool create_swap_chain(const LunaVoxalEngine::Renderer::Device *device, VkSwapchainKHR *swap_chain,
                              VkSurfaceKHR surface) noexcept
{
    // Query surface capabilities, formats, and present modes first
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->physical_device, surface, &capabilities);

    uint32_t format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device->physical_device, surface, &format_count, nullptr);
    VkSurfaceFormatKHR surface_formats[format_count];
    vkGetPhysicalDeviceSurfaceFormatsKHR(device->physical_device, surface, &format_count, surface_formats);

    uint32_t present_mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device->physical_device, surface, &present_mode_count, nullptr);
    VkPresentModeKHR present_modes[present_mode_count];
    vkGetPhysicalDeviceSurfacePresentModesKHR(device->physical_device, surface, &present_mode_count, present_modes);
    // Choose swap chain image count (usually want triple buffering)
    uint32_t image_count = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0)
    {
        image_count = std::min(image_count, capabilities.maxImageCount);
    }
    // TODO: handle different family indices
    uint32_t graphics_family_index = device->graphics_family_index;
    uint32_t present_family_index = device->graphics_family_index;
    VkSwapchainCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.surface = surface;
    create_info.minImageCount = image_count;
    const auto format = chooseSwapSurfaceFormat(surface_formats, format_count);
    create_info.imageFormat = format.format;
    create_info.imageColorSpace = format.colorSpace;
    create_info.imageExtent = capabilities.currentExtent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    uint32_t queue_family_indices[] = {graphics_family_index, present_family_index};
    if (graphics_family_index != present_family_index)
    {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_family_indices;
    }
    else
    {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0;
        create_info.pQueueFamilyIndices = nullptr;
    }
    create_info.preTransform = capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = chooseSwapPresentMode(present_modes, present_mode_count);
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = *swap_chain;

    return vkCreateSwapchainKHR(volkGetLoadedDevice(), &create_info, nullptr, swap_chain) == VK_SUCCESS;
}

namespace LunaVoxalEngine::Renderer
{
SwapChain::SwapChain(const Device *device, void *native_window)
{
#if defined(ON_WINDOWS) || defined(ON_XBOX)
    if (auto result = vkCreateWin32SurfaceKHR(volkGetLoadedInstance(), nullptr, nullptr, &surface);
        result != VK_SUCCESS)
    {
        Log::error("Failed to create win32 surface");
    }
#endif
#if defined(ON_ANDROID)
    if (auto result = vkCreateAndroidSurfaceKHR(volkGetLoadedInstance(), nullptr, nullptr, &surface);
        result != VK_SUCCESS)
    {
        Log::error("Failed to create android surface");
    }
#endif
#if defined(ON_LINUX)
    struct wayland_native
    {
        struct wl_display *display;
        struct wl_surface *surface;
    } *native_window_ = static_cast<struct wayland_native *>(native_window);

    VkWaylandSurfaceCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.display = native_window_->display;
    create_info.surface = native_window_->surface;

    if (auto result = vkCreateWaylandSurfaceKHR(volkGetLoadedInstance(), &create_info, nullptr, &surface);
        result != VK_SUCCESS)
    {
        Log::error("Failed to create wayland surface");
    }
#endif
    if (!create_swap_chain(device, &swap_chain, surface))
    {
        Log::error("Failed to create swapchain");
        throw std::runtime_error("Failed to create swapchain");
    }
}

void SwapChain::resize(const Device *device)
{
    auto old_swap_chain = swap_chain;
    create_swap_chain(device, &swap_chain, surface);
    vkDestroySwapchainKHR(volkGetLoadedDevice(), old_swap_chain, nullptr);
}

SwapChain::~SwapChain()
{
    vkDestroySwapchainKHR(volkGetLoadedDevice(), swap_chain, nullptr);
    vkDestroySurfaceKHR(volkGetLoadedInstance(), surface, nullptr);
}
} // namespace libCross::graphics::vk
