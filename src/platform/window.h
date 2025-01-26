#ifndef PLATFORM_WINDOW_H
#define PLATFORM_WINDOW_H
#include <string>

namespace LunaVoxalEngine::Platform::Window
{
class Window final
{
  public:
    Window(unsigned int width, unsigned int height, const std::string &title);
    void pollEvents();
    void show();
    bool shouldClose() const;
    void hide();
    void resize(unsigned int width, unsigned int height);
    bool isVisible() const;
    void* getvulkanLink();
    ~Window();

  private:
    void *window;
    bool visible;
};

class ChildWindow final
{
  public:
    ChildWindow(unsigned int width, unsigned int height, const std::string &title, Window *parent);
    void embed();
    void pop();
    void pollEvents();
    void show();
    bool shouldClose() const;
    void hide();
    void resize(unsigned int width, unsigned int height);
    bool isVisible() const;
    ~ChildWindow();

  private:
    Window *parent;
    void *window;
    bool visible;
    bool embedded;

    friend Window;
};
} // namespace LunaVoxalEngine::Platform::Window

#endif
