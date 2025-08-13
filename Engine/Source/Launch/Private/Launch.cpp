#include "Engine/Core/Crash/CrashHandler.hpp"
#include "Engine/Core/Logging/Logger.hpp"
#include "Engine/Engine/Engine.hpp"

#include <Windows.h>
void GuardedMain()
{
    GetEngine()->Initialize();
    GetEngine()->Shutdown();
    FEngine::DestroyInstance();
}

#ifdef CORVUS_MODE_SHIPPING
int APIENTRY WinMain(HINSTANCE InstanceHandle, HINSTANCE PreviousInstanceHandle, PSTR CommandLine, int ShowCmd)
#else
int main()
#endif
{
    FLogger::Setup();
    FCrashHandler::Setup();
    TRY
    {
        GuardedMain();
    }
    CATCH()
    {
    }
    const int32 ExitCode = FCrashHandler::GetExitCode();
    FLogger::Destroy();
    return ExitCode;
}
