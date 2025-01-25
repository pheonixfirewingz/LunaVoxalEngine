#ifndef COMMON_MAIN_H
#define COMMON_MAIN_H
#include <vector>
#include <string>
namespace LunaVoxalEngine::Platform
{
class Runtime final
{
  public:
    bool Init(std::vector<std::string> args) noexcept;
    bool IsRunning() const noexcept;
    void Update() noexcept;
    void Shutdown() noexcept;
    static Runtime *Get() noexcept
    {
        static Runtime *runtime;
        [[unlikely]] if (!runtime)
        {
            runtime = new Runtime();
        }
        return runtime;
    }
    static void _Shutdown() noexcept
    {
        Runtime::Get()->Shutdown();
    }
  private:
};
} // namespace LunaVoxalEngine::Platform
#endif