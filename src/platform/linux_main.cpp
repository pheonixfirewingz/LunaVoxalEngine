#include <platform/log.h>
#include <platform/main.h>
using namespace LunaVoxalEngine::Platform;
using namespace LunaVoxalEngine::Log;

int main(int argc, char *argv[])
{
    LunaVoxalEngine::Utils::Vector<LunaVoxalEngine::Utils::String> args;
    args.reserve(argc);
    for (int i = 0; i < argc; i++)
    {
        args.emplace_back(argv[i]);
    }
    debug("Hello from LunaVoxalEngine");
    LunaVoxalEngine::Platform::Runtime *runtime = LunaVoxalEngine::Platform::Runtime::Get();
    debug("Runtime initializing...");
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
    return -1;
}