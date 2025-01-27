#include <platform/log.h>

#include <optional>
#include <stdexcept>
#include <renderer/vulkan/device.h>

using namespace LunaVoxalEngine::Log;

static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                             const VkAllocationCallbacks *pAllocator,
                                             VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    static auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                          const VkAllocationCallbacks *pAllocator)
{
    static auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}

static std::string msgTypeToString(VkDebugUtilsMessageTypeFlagsEXT type) noexcept
{
    switch (type)
    {
    case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
        return "General";
    case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
        return "Validation";
    case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
        return "Performance";
    default:
        return "Unknown";
    }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                    void *) noexcept
{
    const auto type = msgTypeToString(messageType);
    switch (messageSeverity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        error("Validation layer: <TYPE:%s> %s",type.c_str(), pCallbackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        warn("Validation layer: <TYPE:%s> %s", type.c_str(), pCallbackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        info("Validation layer: <TYPE:%s> %s", type.c_str(), pCallbackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        trace("Validation layer: <TYPE:%s> %s", type.c_str(), pCallbackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
        [[fallthrough]];
    default:
        break;
    }
    return VK_FALSE;
}

static bool is_device_suitable(VkPhysicalDevice device, int *score = nullptr) noexcept
{
    VkPhysicalDeviceProperties device_properties;
    VkPhysicalDeviceFeatures device_features;
    vkGetPhysicalDeviceProperties(device, &device_properties);
    vkGetPhysicalDeviceFeatures(device, &device_features);
    int device_score = 0;
    if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        device_score += 10000;
    }
    if (device_features.tessellationShader)
    {
        device_score += 1000;
    }
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(device, &mem_properties);
    VkDeviceSize total_memory = 0;
    for (uint32_t i = 0; i < mem_properties.memoryHeapCount; i++)
    {
        if (mem_properties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
        {
            total_memory += mem_properties.memoryHeaps[i].size;
        }
    }
    device_score += static_cast<int>(total_memory / (1024 * 1024 * 1024));
    if (device_features.geometryShader)
        device_score += 500;
    if (device_features.multiViewport)
        device_score += 200;
    if (device_features.fillModeNonSolid)
        device_score += 100;
    bool is_minimum_requirements_met =
        device_features.tessellationShader && device_properties.limits.maxImageDimension2D >= 4096;
    if (score != nullptr)
    {
        *score = device_score;
    }
    return is_minimum_requirements_met && (device_score > 0);
}

static std::optional<VkPhysicalDevice> pick_physical_device(const VkPhysicalDevice devices[],
                                                            const uint32_t device_count) noexcept
{
    int best_score = 0;
    VkPhysicalDevice best_device = VK_NULL_HANDLE;
    for (uint32_t i = 0; i < device_count; ++i)
    {
        auto device = devices[i];
        int score = 0;
        if (is_device_suitable(device, &score) && score > best_score)
        {
            best_score = score;
            best_device = device;
        }
    }
    if (best_device != VK_NULL_HANDLE)
    {
        return best_device;
    }
    return {};
}

static uint32_t find_queue_families(VkPhysicalDevice device) noexcept
{
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
    VkQueueFamilyProperties queue_families[queue_family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families);
    for (uint32_t i = 0; i < queue_family_count; ++i)
    {
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            return i;
        }
    }
    return UINT32_MAX;
}

namespace LunaVoxalEngine::Renderer
{
Device::Device(const bool debug)
{
    VkInstance instance;
    {
        if (auto result = volkInitialize(); result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to initialize volk");
        }
        VkApplicationInfo app_info = {};
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName = "LunaVoxalEngine";
        app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.pEngineName = "LunaVoxalEngine";
        app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        app_info.apiVersion = VK_API_VERSION_1_3;
        const char *extensions[3] = {VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(ON_WINDOWS) || defined(ON_XBOX)
                                     VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
#if defined(ON_ANDROID)
                                     VK_KHR_ANDROID_SURFACE_EXTENSION_NAME,
#endif
#if defined(ON_LINUX)
                                     VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
#endif
                                     ""};
        VkInstanceCreateInfo create_info = {};
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pApplicationInfo = &app_info;
        create_info.enabledLayerCount = 0;

        VkDebugUtilsMessengerCreateInfoEXT debug_info{};
        if (debug)
        {
            extensions[2] = reinterpret_cast<const char *>(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            create_info.enabledExtensionCount = 3;
            create_info.enabledLayerCount = 1;
            const char *debug_layer_name = "VK_LAYER_KHRONOS_validation";
            create_info.ppEnabledLayerNames = &debug_layer_name;
            debug_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            debug_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                         VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            debug_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            debug_info.pfnUserCallback = debugCallback;
            create_info.pNext = reinterpret_cast<VkDebugUtilsMessengerCreateInfoEXT *>(&debug_info);
        }
        else
        {
            create_info.enabledExtensionCount = 2;
        }
        create_info.ppEnabledExtensionNames = extensions;
        
        if (auto result = vkCreateInstance(&create_info, nullptr, &instance); result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Vulkan instance");
        }

        if (debug)
        {
            if (auto result = CreateDebugUtilsMessengerEXT(instance, &debug_info, nullptr, &debug_messenger);
                result != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create debug messenger");
            }
        }

        volkLoadInstanceOnly(instance);
    }
    uint32_t count = 0;
    vkEnumeratePhysicalDevices(instance, &count, nullptr);
    VkPhysicalDevice devices[count];
    vkEnumeratePhysicalDevices(instance, &count, devices);
    auto pd = pick_physical_device(devices, count);
    if (!pd.has_value())
    {
        throw std::runtime_error("Failed to find a suitable GPU");
    }
    physical_device = pd.value();
    float queue_priority = 1.0f;
    VkDeviceQueueCreateInfo queue_create_info{};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = graphics_family_index = find_queue_families(physical_device);
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;

    VkPhysicalDeviceFeatures device_features{};
    device_features.samplerAnisotropy = VK_TRUE;
    device_features.geometryShader = VK_TRUE;
    device_features.tessellationShader = VK_TRUE;

    VkDeviceCreateInfo device_create_info{};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.queueCreateInfoCount = 1;
    device_create_info.pQueueCreateInfos = &queue_create_info;
    device_create_info.pEnabledFeatures = &device_features;
    const char *extensions[2] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
    };
    device_create_info.enabledExtensionCount = 2;
    device_create_info.ppEnabledExtensionNames = &extensions[0];
    device_create_info.enabledLayerCount = 0;

    VkPhysicalDeviceDynamicRenderingFeatures dr_features = {};
    dr_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
    dr_features.dynamicRendering = VK_TRUE;
    device_create_info.pNext = &dr_features;

    VkDevice device;
    if (auto result = vkCreateDevice(physical_device, &device_create_info, nullptr, &device); result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create logical device");
    }
    volkLoadDevice(device);
    vkGetDeviceQueue(device, queue_create_info.queueFamilyIndex, 0, &graphics_queue);
}

Device::~Device()
{
    auto instance = volkGetLoadedInstance();
    auto device = volkGetLoadedDevice();
    vkDestroyDevice(device, nullptr);
    if (debug_messenger != VK_NULL_HANDLE)
    {
        DestroyDebugUtilsMessengerEXT(instance, debug_messenger, nullptr);
    }
    vkDestroyInstance(instance, nullptr);
    volkFinalize();
}
} // namespace libCross::graphics::vk
