#define WINAPI __stdcall

extern "C" {
    void* WINAPI GetCommandLine(void);
    void* WINAPI CommandLineToArgvW(void* lpCmdLine, int* pNumArgs);
    void* WINAPI LocalAlloc(int flags, size_t size);
    void  WINAPI LocalFree(void* ptr);
}

#include <platform/log.h>
#include <platform/main.h>

using namespace LunaVoxalEngine::Platform;
using namespace LunaVoxalEngine::Log;

int WINAPI WinMain(void* hInstance, void* hPrevInstance, char* lpCmdLine, int nCmdShow)
{
    int argc = 0;
    char** argv = nullptr;
    void* commandLine = GetCommandLine();
    argv = (char**)CommandLineToArgvW(commandLine, &argc);

    std::vector<std::string> args;
    args.reserve(argc);

    for (int i = 0; i < argc; i++)
    {
        args.emplace_back(argv[i]);
    }

    debug("Hello from LunaVoxalEngine");

    LunaVoxalEngine::Platform::Runtime* runtime = LunaVoxalEngine::Platform::Runtime::Get();
    debug("Runtime initializing...");
    if (runtime->Init(args))
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
