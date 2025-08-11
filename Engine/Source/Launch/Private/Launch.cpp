#include "Engine/Core/Crash/CrashHandler.hpp"
#include "Engine/Engine/Engine.hpp"

void GuardedMain()
{
    GetEngine()->Initialize();
    GetEngine()->Shutdown();
    FEngine::DestroyInstance();
}

int main()
{
    FCrashHandler::Setup();
    TRY
    {
        GuardedMain();
    }
    CATCH()
    {
    }
    return FCrashHandler::GetExitCode();
}
