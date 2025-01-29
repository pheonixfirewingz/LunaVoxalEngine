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
#if defined(USE_WAYLAND)
#    define _NEW
#    include <wayland-client.h>
#    undef _NEW
#include <vulkan/vulkan_wayland.h>
#else
#    include <xcb/xcb.h>
#include <vulkan/vulkan_xcb.h>
#endif
#endif
#include <volk.h>
#undef VK_NO_PROTOTYPES
#endif