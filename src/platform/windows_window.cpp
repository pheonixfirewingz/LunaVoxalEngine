#include <platform/window.h>

#include <windows.h>

namespace LunaVoxelEngine
{
namespace Platform
{

struct __WindowData
{
    HWND hwnd;
    HINSTANCE hInstance;
};

Window::Window(unsigned int width, unsigned int height, const Utils::String &title)
{
    window = new __WindowData;
    __WindowData *data = static_cast<__WindowData *>(window);
    data->hInstance = GetModuleHandle(nullptr);

    WNDCLASS wc = {};
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = data->hInstance;
    wc.lpszClassName = reinterpret_cast<LPCSTR>(L"LunaVoxelEngineWindow");
    RegisterClass(&wc);

    const auto str = title.throw_away();
    data->hwnd = CreateWindowEx(0, wc.lpszClassName, str.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                                width, height, nullptr, nullptr, data->hInstance, nullptr);
    visible = false;
}

void Window::pollEvents()
{
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void Window::show()
{
    __WindowData *data = static_cast<__WindowData *>(window);
    ShowWindow(data->hwnd, SW_SHOW);
    visible = true;
}

bool Window::shouldClose() const
{
    return false; // No internal close flag tracking yet
}

void Window::hide()
{
    __WindowData *data = static_cast<__WindowData *>(window);
    ShowWindow(data->hwnd, SW_HIDE);
    visible = false;
}

void Window::resize(unsigned int width, unsigned int height)
{
    __WindowData *data = static_cast<__WindowData *>(window);
    SetWindowPos(data->hwnd, nullptr, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
}

bool Window::isVisible() const
{
    return visible;
}

NativeWindow Window::getVulkanLink()
{
    __WindowData *data = static_cast<__WindowData *>(window);
    return {data->hwnd, data->hInstance};
}

Window::~Window()
{
    __WindowData *data = static_cast<__WindowData *>(window);
    DestroyWindow(data->hwnd);
    delete data;
}

ChildWindow::ChildWindow(unsigned int width, unsigned int height, const Utils::String &title, Window *parent_in)
    : parent(parent_in)
{
    window = new __WindowData;
    __WindowData *data = static_cast<__WindowData *>(window);
    data->hInstance = GetModuleHandle(nullptr);

    WNDCLASS wc = {};
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = data->hInstance;
    wc.lpszClassName = reinterpret_cast<LPCSTR>(L"LunaVoxelEngineChildWindow");
    RegisterClass(&wc);

    const auto str = title.throw_away();
    data->hwnd =
        CreateWindowEx(0, wc.lpszClassName, str.c_str(), WS_CHILD | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, width,
                       height, static_cast<__WindowData *>(parent->window)->hwnd, nullptr, data->hInstance, nullptr);
    visible = false;
    embedded = true;
}

void ChildWindow::embed()
{
    embedded = true;
}

void ChildWindow::pop()
{
    embedded = false;
}

void ChildWindow::pollEvents()
{
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void ChildWindow::show()
{
    __WindowData *data = static_cast<__WindowData *>(window);
    ShowWindow(data->hwnd, SW_SHOW);
    visible = true;
}

bool ChildWindow::shouldClose() const
{
    return false;
}

void ChildWindow::hide()
{
    __WindowData *data = static_cast<__WindowData *>(window);
    ShowWindow(data->hwnd, SW_HIDE);
    visible = false;
}

void ChildWindow::resize(unsigned int width, unsigned int height)
{
    __WindowData *data = static_cast<__WindowData *>(window);
    SetWindowPos(data->hwnd, nullptr, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
}

bool ChildWindow::isVisible() const
{
    return visible;
}

NativeWindow ChildWindow::getVulkanLink()
{
    __WindowData *data = static_cast<__WindowData *>(window);
    return {data->hwnd, data->hInstance};
}

ChildWindow::~ChildWindow()
{
    __WindowData *data = static_cast<__WindowData *>(window);
    DestroyWindow(data->hwnd);
    delete data;
}

} // namespace Platform
} // namespace LunaVoxelEngine