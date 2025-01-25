#include <platform/main.h>
#include <platform/Log.h>

using namespace LunaVoxalEngine::Platform;
using namespace LunaVoxalEngine::Log;

int main(int argc, char *argv[])
{
    std::vector<std::string> args;
    args.reserve(argc);
    for (int i = 0; i < argc; i++)
    {
        args.emplace_back(argv[i]);
    }
    debug("Hello from LunaVoxalEngine");
    LunaVoxalEngine::Platform::Runtime* runtime = LunaVoxalEngine::Platform::Runtime::Get();
    debug("Runtime initialized");
    if (!runtime->Init(args))
    {
        debug("Runtime initialized");
        while (runtime->IsRunning())
        {
            runtime->Update();
        }
        runtime->Shutdown();
        debug("Runtime shutdown");
        return 0;
        
    }
    error("Error while initializing runtime");
    return 0;
}