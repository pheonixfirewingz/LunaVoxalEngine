#include <platform/log.h>
#include <platform/main.h>
#include <shellapi.h>
#include <utils/string.h>
#include <utils/vector.h>
#include <windows.h>

using namespace LunaVoxelEngine::Platform;
using namespace LunaVoxelEngine::Log;
using namespace LunaVoxelEngine::Utils;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{

    int argc = 0;
    LPWSTR *argvW = CommandLineToArgvW(GetCommandLineW(), &argc);
    Vector<String> args;
    args.reserve(argc);
    for (int i = 0; i < argc; i++)
    {
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, NULL, 0, NULL, NULL);
        char *arg = new char[size_needed];
        WideCharToMultiByte(CP_UTF8, 0, argvW[i], -1, &arg[0], size_needed, NULL, NULL);
        args.emplace_back(arg);
        delete[] arg;
    }
    LocalFree(argvW);

    debug("Hello from LunaVoxelEngine");
    LunaVoxelEngine::Platform::Runtime *runtime = LunaVoxelEngine::Platform::Runtime::Get();
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
