#include <platform/log.h>

#include <renderer/vulkan/swapchain.h>
#include <utils/algoritom.h>

using namespace LunaVoxalEngine::Utils;

extern VkAllocationCallbacks callbacks;

static inline const char *string_VkResult(VkResult input_value)
{
    switch (input_value)
    {
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

static VkResult create_swap_chain(const LunaVoxalEngine::Renderer::Device *device, VkSwapchainKHR *swap_chain,
                                  VkSurfaceKHR surface, VkSurfaceFormatKHR format, VkPresentModeKHR present_mode,
                                  VkExtent2D *extent) noexcept
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device->get_physical_device(), surface, &capabilities);

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
    create_info.surface = surface;
    create_info.minImageCount = image_count;
    create_info.imageFormat = format.format;
    create_info.imageColorSpace = format.colorSpace;
    create_info.imageExtent = capabilities.currentExtent;
    *extent = capabilities.currentExtent; // Store the extent
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
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = *swap_chain;

    return vkCreateSwapchainKHR(volkGetLoadedDevice(), &create_info, &callbacks, swap_chain);
}

static void create_image_views(VkSwapchainKHR swap_chain, VkSurfaceFormatKHR format, Vector<VkImage> *images,
                               Vector<VkImageView> *image_views) noexcept
{
    uint32_t image_count = 0;
    vkGetSwapchainImagesKHR(volkGetLoadedDevice(), swap_chain, &image_count, nullptr);
    images->resize(image_count);
    vkGetSwapchainImagesKHR(volkGetLoadedDevice(), swap_chain, &image_count, images->data());

    image_views->resize(image_count);
    for (uint32_t i = 0; i < image_count; i++)
    {
        VkImageViewCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        create_info.image = images->at(i);
        create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        create_info.format = format.format;
        create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        create_info.subresourceRange.baseMipLevel = 0;
        create_info.subresourceRange.levelCount = 1;
        create_info.subresourceRange.baseArrayLayer = 0;
        create_info.subresourceRange.layerCount = 1;
        if (vkCreateImageView(volkGetLoadedDevice(), &create_info, &callbacks, &image_views->at(i)) != VK_SUCCESS)
        {
            LunaVoxalEngine::Log::fatal("Failed to create image views!");
        }
    }
}

namespace LunaVoxalEngine::Renderer
{

VkResult SwapChain::acquireNextImage(VkSemaphore semaphore, VkFence fence, uint32_t *imageIndex)
{
    return vkAcquireNextImageKHR(volkGetLoadedDevice(), swap_chain, UINT64_MAX, semaphore, fence, imageIndex);
}

VkResult SwapChain::present(Queue* queue, uint32_t imageIndex, VkSemaphore waitSemaphore)
{
    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &waitSemaphore;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &swap_chain;
    present_info.pImageIndices = &imageIndex;
    return queue->present(present_info);
}

SwapChain::SwapChain(const Device *device, Platform::NativeWindow native_window)
{
#if defined(ON_WINDOWS) || defined(ON_XBOX)
    struct win32_native
    {
        HINSTANCE hinstance;
        HWND hwnd;
    } *native_window_ = static_cast<struct win32_native *>(native_window);

    VkWin32SurfaceCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    create_info.hinstance = native_window_->hinstance;
    create_info.hwnd = native_window_->hwnd;

    if (auto result = vkCreateWin32SurfaceKHR(volkGetLoadedInstance(), &create_info, &callbacks, &surface);
        result != VK_SUCCESS)
    {
        Log::error("Failed to create win32 surface %s", string_VkResult(result));
    }
#endif
#if defined(ON_ANDROID)
    struct android_native
    {
        ANativeWindow *native_window;
    } *native_window_ = static_cast<struct android_native *>(native_window);

    VkAndroidSurfaceCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    create_info.window = native_window_->native_window;

    if (auto result = vkCreateAndroidSurfaceKHR(volkGetLoadedInstance(), &create_info, &callbacks, &surface);
        result != VK_SUCCESS)
    {
        Log::error("Failed to create android surface");
    }
#endif
#if defined(ON_LINUX)
#    if defined(USE_WAYLAND)
    VkWaylandSurfaceCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
    create_info.display = static_cast<wl_display *>(native_window.window);
    create_info.surface = static_cast<wl_surface *>(native_window.other);

    if (auto result = vkCreateWaylandSurfaceKHR(volkGetLoadedInstance(), &create_info, &callbacks, &surface);
        result != VK_SUCCESS)
    {
        Log::error("Failed to create wayland surface %s", string_VkResult(result));
    }
#    else
    VkXcbSurfaceCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    create_info.connection = static_cast<xcb_connection_t *>(native_window.other);
    create_info.window = *static_cast<xcb_window_t *>(native_window.window);

    if (auto result = vkCreateXcbSurfaceKHR(volkGetLoadedInstance(), &create_info, &callbacks, &surface);
        result != VK_SUCCESS)
    {
        Log::error("Failed to create xcb surface");
    }
#    endif
#endif
    uint32_t format_count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device->get_physical_device(), surface, &format_count, nullptr);
    VkSurfaceFormatKHR surface_formats[format_count];
    vkGetPhysicalDeviceSurfaceFormatsKHR(device->get_physical_device(), surface, &format_count, surface_formats);
    auto chose = [&]() {
        for (uint32_t i = 0; i < format_count; i++)
        {
            VkSurfaceFormatKHR availableFormat = surface_formats[i];
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }
        return surface_formats[0];
    };

    uint32_t present_mode_count = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device->get_physical_device(), surface, &present_mode_count, nullptr);
    VkPresentModeKHR present_modes[present_mode_count];
    vkGetPhysicalDeviceSurfacePresentModesKHR(device->get_physical_device(), surface, &present_mode_count,
                                              present_modes);
    auto chose_present_mode = [&]() {
        for (uint32_t i = 0; i < present_mode_count; i++)
        {
            if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return present_modes[i];
            }
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    };

    image_format = chose();
    present_mode = chose_present_mode();

    if (auto result = create_swap_chain(device, &swap_chain, surface, image_format, present_mode, &extent);
        result != VK_SUCCESS)
    {
        Log::error("Failed to create swapchain %s", string_VkResult(result));
    }
    create_image_views(swap_chain, image_format, &images, &image_views);
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
    for (auto image_view : image_views)
    {
        vkDestroyImageView(volkGetLoadedDevice(), image_view, &callbacks);
    }
    create_swap_chain(device, &swap_chain, surface, image_format, present_mode, &extent);
    vkDestroySwapchainKHR(volkGetLoadedDevice(), old_swap_chain, &callbacks);
    create_image_views(swap_chain, image_format, &images, &image_views);
}

SwapChain::~SwapChain()
{
    for (auto image_view : image_views)
    {
        vkDestroyImageView(volkGetLoadedDevice(), image_view, &callbacks);
    }
    vkDestroySwapchainKHR(volkGetLoadedDevice(), swap_chain, &callbacks);
    vkDestroySurfaceKHR(volkGetLoadedInstance(), surface, &callbacks);
}

} // namespace LunaVoxalEngine::Renderer