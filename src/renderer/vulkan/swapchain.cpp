#include <platform/log.h>

#include <renderer/vulkan/swapchain.h>
#include <utils/algoritom.h>
static inline const char* string_VkResult(VkResult input_value) {
    switch (input_value) {
        case VK_SUCCESS:
            return "VK_SUCCESS";
        case VK_NOT_READY:
            return "VK_NOT_READY";
        case VK_TIMEOUT:
            return "VK_TIMEOUT";
        case VK_EVENT_SET:
            return "VK_EVENT_SET";
        case VK_EVENT_RESET:
            return "VK_EVENT_RESET";
        case VK_INCOMPLETE:
            return "VK_INCOMPLETE";
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return "VK_ERROR_OUT_OF_HOST_MEMORY";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        case VK_ERROR_INITIALIZATION_FAILED:
            return "VK_ERROR_INITIALIZATION_FAILED";
        case VK_ERROR_DEVICE_LOST:
            return "VK_ERROR_DEVICE_LOST";
        case VK_ERROR_MEMORY_MAP_FAILED:
            return "VK_ERROR_MEMORY_MAP_FAILED";
        case VK_ERROR_LAYER_NOT_PRESENT:
            return "VK_ERROR_LAYER_NOT_PRESENT";
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            return "VK_ERROR_EXTENSION_NOT_PRESENT";
        case VK_ERROR_FEATURE_NOT_PRESENT:
            return "VK_ERROR_FEATURE_NOT_PRESENT";
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            return "VK_ERROR_INCOMPATIBLE_DRIVER";
        case VK_ERROR_TOO_MANY_OBJECTS:
            return "VK_ERROR_TOO_MANY_OBJECTS";
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            return "VK_ERROR_FORMAT_NOT_SUPPORTED";
        case VK_ERROR_FRAGMENTED_POOL:
            return "VK_ERROR_FRAGMENTED_POOL";
        case VK_ERROR_UNKNOWN:
            return "VK_ERROR_UNKNOWN";
        case VK_ERROR_OUT_OF_POOL_MEMORY:
            return "VK_ERROR_OUT_OF_POOL_MEMORY";
        case VK_ERROR_INVALID_EXTERNAL_HANDLE:
            return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
        case VK_ERROR_FRAGMENTATION:
            return "VK_ERROR_FRAGMENTATION";
        case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
            return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
        case VK_PIPELINE_COMPILE_REQUIRED:
            return "VK_PIPELINE_COMPILE_REQUIRED";
        case VK_ERROR_NOT_PERMITTED:
            return "VK_ERROR_NOT_PERMITTED";
        case VK_ERROR_SURFACE_LOST_KHR:
            return "VK_ERROR_SURFACE_LOST_KHR";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        case VK_SUBOPTIMAL_KHR:
            return "VK_SUBOPTIMAL_KHR";
        case VK_ERROR_OUT_OF_DATE_KHR:
            return "VK_ERROR_OUT_OF_DATE_KHR";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
            return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
        case VK_ERROR_VALIDATION_FAILED_EXT:
            return "VK_ERROR_VALIDATION_FAILED_EXT";
        case VK_ERROR_INVALID_SHADER_NV:
            return "VK_ERROR_INVALID_SHADER_NV";
        case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR:
            return "VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR";
        case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR:
            return "VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR";
        case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR:
            return "VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR";
        case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR:
            return "VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR";
        case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR:
            return "VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR";
        case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR:
            return "VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR";
        case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
            return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
        case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
            return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
        case VK_THREAD_IDLE_KHR:
            return "VK_THREAD_IDLE_KHR";
        case VK_THREAD_DONE_KHR:
            return "VK_THREAD_DONE_KHR";
        case VK_OPERATION_DEFERRED_KHR:
            return "VK_OPERATION_DEFERRED_KHR";
        case VK_OPERATION_NOT_DEFERRED_KHR:
            return "VK_OPERATION_NOT_DEFERRED_KHR";
        case VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR:
            return "VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR";
        case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
            return "VK_ERROR_COMPRESSION_EXHAUSTED_EXT";
        case VK_INCOMPATIBLE_SHADER_BINARY_EXT:
            return "VK_INCOMPATIBLE_SHADER_BINARY_EXT";
        case VK_PIPELINE_BINARY_MISSING_KHR:
            return "VK_PIPELINE_BINARY_MISSING_KHR";
        case VK_ERROR_NOT_ENOUGH_SPACE_KHR:
            return "VK_ERROR_NOT_ENOUGH_SPACE_KHR";
        default:
            return "Unhandled VkResult";
    }
}

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const VkSurfaceFormatKHR *availableFormats, uint32_t formatCount)
{
    for (uint32_t i = 0; i < formatCount; i++)
    {
        VkSurfaceFormatKHR availableFormat = availableFormats[i];
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

VkPresentModeKHR chooseSwapPresentMode(const VkPresentModeKHR *availablePresentModes, uint32_t presentModeCount)
{
    for (uint32_t i = 0; i < presentModeCount; i++)
    {
        VkPresentModeKHR availablePresentMode = availablePresentModes[i];
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

static VkResult create_swap_chain(const LunaVoxalEngine::Renderer::Device *device, VkSwapchainKHR *swap_chain,
                              VkSurfaceKHR surface) noexcept
{
    // Query surface capabilities, formats, and present modes first
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->get_physical_device(), surface, &capabilities);

    uint32_t format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device->get_physical_device(), surface, &format_count, nullptr);
    VkSurfaceFormatKHR surface_formats[format_count];
    vkGetPhysicalDeviceSurfaceFormatsKHR(device->get_physical_device(), surface, &format_count, surface_formats);

    uint32_t present_mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device->get_physical_device(), surface, &present_mode_count, nullptr);
    VkPresentModeKHR present_modes[present_mode_count];
    vkGetPhysicalDeviceSurfacePresentModesKHR(device->get_physical_device(), surface, &present_mode_count,
                                              present_modes);
    // Choose swap chain image count (usually want triple buffering)
    uint32_t image_count = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0)
    {
        image_count = LunaVoxalEngine::Utils::min(image_count, capabilities.maxImageCount);
    }
    // TODO: handle different family indices
    uint32_t graphics_family_index = device->get_graphics_family_index();
    uint32_t present_family_index = device->get_graphics_family_index();
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

    return vkCreateSwapchainKHR(volkGetLoadedDevice(), &create_info, nullptr, swap_chain);
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
        Log::error("Failed to create wayland surface %s", string_VkResult(result));
    }
#endif
    if (auto result = create_swap_chain(device, &swap_chain, surface); result != VK_SUCCESS)
    {
        Log::error("Failed to create swapchain %s", string_VkResult(result));
    }
}

void SwapChain::resize(const Device *device)
{
#ifdef DEBUG
    if (swap_chain == VK_NULL_HANDLE)
    {
        Log::fatal("Swap chain is not created");
    }
#endif
    auto old_swap_chain = swap_chain;
    create_swap_chain(device, &swap_chain, surface);
    vkDestroySwapchainKHR(volkGetLoadedDevice(), old_swap_chain, nullptr);
}

SwapChain::~SwapChain()
{
    vkDestroySwapchainKHR(volkGetLoadedDevice(), swap_chain, nullptr);
    vkDestroySurfaceKHR(volkGetLoadedInstance(), surface, nullptr);
}
} // namespace LunaVoxalEngine::Renderer
