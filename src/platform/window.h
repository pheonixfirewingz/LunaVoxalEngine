#ifndef PLATFORM_WINDOW_H
#define PLATFORM_WINDOW_H
#include <utils/string.h>
namespace LunaVoxelEngine
{
namespace  Platform
{

struct NativeWindow
{
    void *window;
    void *other;
};

class Window final
{
  public:
    Window(unsigned int width, unsigned int height, const Utils::String &title);
    void pollEvents();
    void show();
    bool shouldClose() const;
    void hide();
    void resize(unsigned int width, unsigned int height);
    bool isVisible() const;
    NativeWindow getVulkanLink();
    ~Window();

  private:
    void *window;
    bool visible;
    friend class ChildWindow;
};

class ChildWindow final
{
  public:
    ChildWindow(unsigned int width, unsigned int height, const Utils::String &title, Window *parent);
    void embed();
    void pop();
    void pollEvents();
    void show();
    bool shouldClose() const;
    void hide();
    void resize(unsigned int width, unsigned int height);
    bool isVisible() const;
    NativeWindow getVulkanLink();
    ~ChildWindow();

  private:
    Window *parent;
    void *window;
    bool visible;
    bool embedded;

    friend Window;
};
} // namespace  Platform
} // namespace LunaVoxelEngine
#endif
