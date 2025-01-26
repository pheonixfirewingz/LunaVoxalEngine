#include <platform/window.h>
#ifdef USE_WAYLAND
#    include <platform/xdg-decoration-unstable-v1-client-protocol.h>
#include <platform/xdg-shell-client-protocol.h>
#    include <wayland-client.h>
#else
#    include <xcb/xcb.h>
#endif
#include <stdexcept>
namespace LunaVoxalEngine::Platform::Window
{
#ifdef USE_WAYLAND
struct __WindowData
{
    wl_display *display;
    wl_registry *registry;
    wl_compositor *compositor;
    wl_surface *surface;
    struct xdg_wm_base *xdg_wm_base;
    struct xdg_toplevel *xdg_toplevel;
    struct xdg_surface *xdg_surface;
    zxdg_decoration_manager_v1 *decoration_manager;
    zxdg_toplevel_decoration_v1 *decoration;
    struct xdg_positioner *positioner;
};

struct __VulkanWayland
{
    wl_display *display;
    wl_surface *surface;
};

Window::Window(unsigned int width, unsigned int height, const std::string &title)
{
    wl_display *display = wl_display_connect(nullptr);
    if (!display)
    {
        throw std::runtime_error("Failed to connect to Wayland server");
    }

    wl_registry *registry = wl_display_get_registry(display);
    if (!registry)
    {
        throw std::runtime_error("Failed to get registry");
    }

    wl_compositor *compositor =
        static_cast<wl_compositor *>(wl_registry_bind(registry, 1, &wl_compositor_interface, 4));
    if (!compositor)
    {
        throw std::runtime_error("Failed to bind wl_compositor");
    }

    wl_surface *surface = wl_compositor_create_surface(compositor);
    if (!surface)
    {
        throw std::runtime_error("Failed to create wl_surface");
    }

    struct xdg_wm_base *xdg_wm_base = static_cast<struct xdg_wm_base *>(wl_registry_bind(registry, 3, &xdg_wm_base_interface, 1));
    if (!xdg_wm_base)
    {
        throw std::runtime_error("Failed to bind xdg_wm_base");
    }

    xdg_surface *xdg_surface = xdg_wm_base_get_xdg_surface(xdg_wm_base, surface);
    if (!xdg_surface)
    {
        throw std::runtime_error("Failed to create xdg_surface");
    }

    xdg_toplevel *xdg_toplevel = xdg_surface_get_toplevel(xdg_surface);
    if (!xdg_toplevel)
    {
        throw std::runtime_error("Failed to create xdg_toplevel");
    }

    xdg_toplevel_set_title(xdg_toplevel, title.c_str());
    std::string app_id = "com.github.LunaVoxalEngine." + title;
    xdg_toplevel_set_app_id(xdg_toplevel, app_id.c_str());

    xdg_positioner *positioner = xdg_wm_base_create_positioner(xdg_wm_base);
    if (!positioner)
    {
        throw std::runtime_error("Failed to create xdg_positioner");
    }
    xdg_positioner_set_size(positioner, width, height);

    zxdg_decoration_manager_v1 *decoration_manager =
        static_cast<zxdg_decoration_manager_v1 *>(wl_registry_bind(registry, 4, &zxdg_decoration_manager_v1_interface, 1));
    if (!decoration_manager)
    {
        throw std::runtime_error("Failed to bind zxdg_decoration_manager_v1");
    }

    zxdg_toplevel_decoration_v1 *decoration =
        zxdg_decoration_manager_v1_get_toplevel_decoration(decoration_manager, xdg_toplevel);
    if (!decoration)
    {
        throw std::runtime_error("Failed to create xdg_toplevel_decoration_v1");
    }

    zxdg_toplevel_decoration_v1_set_mode(decoration, ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE);

    wl_surface_commit(surface);

    window =
        new __WindowData{display, registry, compositor, surface, xdg_wm_base, xdg_toplevel, xdg_surface, decoration_manager, decoration, positioner};
}

void Window::pollEvents()
{
    auto data = (__WindowData *)window;
    wl_display_dispatch_pending(data->display);
}

void Window::show()
{
    auto data = (__WindowData *)window;
    wl_surface_commit(data->surface);
}

void Window::hide()
{
    auto data = (__WindowData *)window;
    wl_surface_commit(data->surface);
}

void Window::resize(unsigned int width, unsigned int height)
{
    auto data = (__WindowData *)window;
    xdg_positioner_set_size(data->positioner, width, height);
    wl_surface_commit(data->surface);
}

bool Window::shouldClose() const
{
    auto data = (__WindowData *)window;
    wl_display_dispatch_pending(data->display);
    return !data->xdg_toplevel;
}

bool Window::isVisible() const
{
    return visible;
}

void *Window::getvulkanLink()
{
    auto data = (__WindowData *)window;
    return new __VulkanWayland {data->display, data->surface};
}

Window::~Window()
{
    auto data = (__WindowData *)window;
    zxdg_toplevel_decoration_v1_destroy(data->decoration);
    xdg_toplevel_destroy(data->xdg_toplevel);
    xdg_surface_destroy(data->xdg_surface);
    wl_surface_destroy(data->surface);
    wl_compositor_destroy(data->compositor);
    wl_registry_destroy(data->registry);
    xdg_positioner_destroy(data->positioner);
    zxdg_decoration_manager_v1_destroy(data->decoration_manager);
}
#else
Window::Window(unsigned int width, unsigned int height, const std::string &title)
{
}

void Window::pollEvents()
{
}
void Window::show()
{
}
void Window::hide()
{
}
void Window::resize(unsigned int width, unsigned int height)
{
}

bool Window::shouldClose() const
{
    return false;
}

void Window::getvulkanLink()
{
    return nullptr;
}

Window::~Window()
{
}
#endif
} // namespace LunaVoxalEngine::Platform::Window
#if defined(USE_WAYLAND) 
/* Generated by wayland-scanner 1.22.0 */

/*
 * https://gitlab.freedesktop.org/wayland/wayland-protocols/-/blob/main/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml acessed (jan 26 2025)
 * Copyright © 2018 Simon Ser
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
extern "C" {
	
#include <stdlib.h>
#include <stdint.h>
#include <wayland-util.h>

#ifndef __has_attribute
# define __has_attribute(x) 0  /* Compatibility with non-clang compilers. */
#endif

#if (__has_attribute(visibility) || defined(__GNUC__) && __GNUC__ >= 4)
#define WL_PRIVATE __attribute__ ((visibility("hidden")))
#else
#define WL_PRIVATE
#endif

extern const struct wl_interface xdg_toplevel_interface;
extern const struct wl_interface zxdg_toplevel_decoration_v1_interface;

static const struct wl_interface *xdg_decoration_unstable_v1_types[] = {
	NULL,
	&zxdg_toplevel_decoration_v1_interface,
	&xdg_toplevel_interface,
};

static const struct wl_message zxdg_decoration_manager_v1_requests[] = {
	{ "destroy", "", xdg_decoration_unstable_v1_types + 0 },
	{ "get_toplevel_decoration", "no", xdg_decoration_unstable_v1_types + 1 },
};

WL_PRIVATE const struct wl_interface zxdg_decoration_manager_v1_interface = {
	"zxdg_decoration_manager_v1", 1,
	2, zxdg_decoration_manager_v1_requests,
	0, NULL,
};

static const struct wl_message zxdg_toplevel_decoration_v1_requests[] = {
	{ "destroy", "", xdg_decoration_unstable_v1_types + 0 },
	{ "set_mode", "u", xdg_decoration_unstable_v1_types + 0 },
	{ "unset_mode", "", xdg_decoration_unstable_v1_types + 0 },
};

static const struct wl_message zxdg_toplevel_decoration_v1_events[] = {
	{ "configure", "u", xdg_decoration_unstable_v1_types + 0 },
};

WL_PRIVATE const struct wl_interface zxdg_toplevel_decoration_v1_interface = {
	"zxdg_toplevel_decoration_v1", 1,
	3, zxdg_toplevel_decoration_v1_requests,
	1, zxdg_toplevel_decoration_v1_events,
};

}

/* Generated by wayland-scanner 1.22.0 */

/*
 * https://gitlab.freedesktop.org/wayland/wayland-protocols/-/blob/main/stable/xdg-shell/xdg-shell.xml accessed (jan 26 2025)
 * Copyright © 2008-2013 Kristian Høgsberg
 * Copyright © 2013      Rafael Antognolli
 * Copyright © 2013      Jasper St. Pierre
 * Copyright © 2010-2013 Intel Corporation
 * Copyright © 2015-2017 Samsung Electronics Co., Ltd
 * Copyright © 2015-2017 Red Hat Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
extern "C" {
#include <stdlib.h>
#include <stdint.h>
#include <wayland-util.h>

#ifndef __has_attribute
# define __has_attribute(x) 0  /* Compatibility with non-clang compilers. */
#endif

#if (__has_attribute(visibility) || defined(__GNUC__) && __GNUC__ >= 4)
#define WL_PRIVATE __attribute__ ((visibility("hidden")))
#else
#define WL_PRIVATE
#endif

extern const struct wl_interface wl_output_interface;
extern const struct wl_interface wl_seat_interface;
extern const struct wl_interface wl_surface_interface;
extern const struct wl_interface xdg_popup_interface;
extern const struct wl_interface xdg_positioner_interface;
extern const struct wl_interface xdg_surface_interface;
extern const struct wl_interface xdg_toplevel_interface;

static const struct wl_interface *xdg_shell_types[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	&xdg_positioner_interface,
	&xdg_surface_interface,
	&wl_surface_interface,
	&xdg_toplevel_interface,
	&xdg_popup_interface,
	&xdg_surface_interface,
	&xdg_positioner_interface,
	&xdg_toplevel_interface,
	&wl_seat_interface,
	NULL,
	NULL,
	NULL,
	&wl_seat_interface,
	NULL,
	&wl_seat_interface,
	NULL,
	NULL,
	&wl_output_interface,
	&wl_seat_interface,
	NULL,
	&xdg_positioner_interface,
	NULL,
};

static const struct wl_message xdg_wm_base_requests[] = {
	{ "destroy", "", xdg_shell_types + 0 },
	{ "create_positioner", "n", xdg_shell_types + 4 },
	{ "get_xdg_surface", "no", xdg_shell_types + 5 },
	{ "pong", "u", xdg_shell_types + 0 },
};

static const struct wl_message xdg_wm_base_events[] = {
	{ "ping", "u", xdg_shell_types + 0 },
};

WL_PRIVATE const struct wl_interface xdg_wm_base_interface = {
	"xdg_wm_base", 6,
	4, xdg_wm_base_requests,
	1, xdg_wm_base_events,
};

static const struct wl_message xdg_positioner_requests[] = {
	{ "destroy", "", xdg_shell_types + 0 },
	{ "set_size", "ii", xdg_shell_types + 0 },
	{ "set_anchor_rect", "iiii", xdg_shell_types + 0 },
	{ "set_anchor", "u", xdg_shell_types + 0 },
	{ "set_gravity", "u", xdg_shell_types + 0 },
	{ "set_constraint_adjustment", "u", xdg_shell_types + 0 },
	{ "set_offset", "ii", xdg_shell_types + 0 },
	{ "set_reactive", "3", xdg_shell_types + 0 },
	{ "set_parent_size", "3ii", xdg_shell_types + 0 },
	{ "set_parent_configure", "3u", xdg_shell_types + 0 },
};

WL_PRIVATE const struct wl_interface xdg_positioner_interface = {
	"xdg_positioner", 6,
	10, xdg_positioner_requests,
	0, NULL,
};

static const struct wl_message xdg_surface_requests[] = {
	{ "destroy", "", xdg_shell_types + 0 },
	{ "get_toplevel", "n", xdg_shell_types + 7 },
	{ "get_popup", "n?oo", xdg_shell_types + 8 },
	{ "set_window_geometry", "iiii", xdg_shell_types + 0 },
	{ "ack_configure", "u", xdg_shell_types + 0 },
};

static const struct wl_message xdg_surface_events[] = {
	{ "configure", "u", xdg_shell_types + 0 },
};

WL_PRIVATE const struct wl_interface xdg_surface_interface = {
	"xdg_surface", 6,
	5, xdg_surface_requests,
	1, xdg_surface_events,
};

static const struct wl_message xdg_toplevel_requests[] = {
	{ "destroy", "", xdg_shell_types + 0 },
	{ "set_parent", "?o", xdg_shell_types + 11 },
	{ "set_title", "s", xdg_shell_types + 0 },
	{ "set_app_id", "s", xdg_shell_types + 0 },
	{ "show_window_menu", "ouii", xdg_shell_types + 12 },
	{ "move", "ou", xdg_shell_types + 16 },
	{ "resize", "ouu", xdg_shell_types + 18 },
	{ "set_max_size", "ii", xdg_shell_types + 0 },
	{ "set_min_size", "ii", xdg_shell_types + 0 },
	{ "set_maximized", "", xdg_shell_types + 0 },
	{ "unset_maximized", "", xdg_shell_types + 0 },
	{ "set_fullscreen", "?o", xdg_shell_types + 21 },
	{ "unset_fullscreen", "", xdg_shell_types + 0 },
	{ "set_minimized", "", xdg_shell_types + 0 },
};

static const struct wl_message xdg_toplevel_events[] = {
	{ "configure", "iia", xdg_shell_types + 0 },
	{ "close", "", xdg_shell_types + 0 },
	{ "configure_bounds", "4ii", xdg_shell_types + 0 },
	{ "wm_capabilities", "5a", xdg_shell_types + 0 },
};

WL_PRIVATE const struct wl_interface xdg_toplevel_interface = {
	"xdg_toplevel", 6,
	14, xdg_toplevel_requests,
	4, xdg_toplevel_events,
};

static const struct wl_message xdg_popup_requests[] = {
	{ "destroy", "", xdg_shell_types + 0 },
	{ "grab", "ou", xdg_shell_types + 22 },
	{ "reposition", "3ou", xdg_shell_types + 24 },
};

static const struct wl_message xdg_popup_events[] = {
	{ "configure", "iiii", xdg_shell_types + 0 },
	{ "popup_done", "", xdg_shell_types + 0 },
	{ "repositioned", "3u", xdg_shell_types + 0 },
};

WL_PRIVATE const struct wl_interface xdg_popup_interface = {
	"xdg_popup", 6,
	3, xdg_popup_requests,
	3, xdg_popup_events,
};
}
#endif