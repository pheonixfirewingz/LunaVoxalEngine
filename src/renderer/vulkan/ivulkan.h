#ifndef IVULKAN_H
#define IVULKAN_H
#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#if defined(ON_WINDOWS) || defined(ON_XBOX)
#include <vulkan/vulkan_win32.h>
#endif
#if defined(ON_ANDROID)
#include <vulkan/vulkan_android.h>
#endif
#if defined(ON_LINUX)
#include <vulkan/vulkan_wayland.h>
#endif
#include <volk.h>
#undef VK_NO_PROTOTYPES
#endif