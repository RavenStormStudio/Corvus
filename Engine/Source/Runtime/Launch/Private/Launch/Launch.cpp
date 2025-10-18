// RavenStorm Copyright @ 2025-2025

#include "Launch/Launch.hpp"

#include "Core/Assertion/ExceptionHandler.hpp"
#include "Core/Logging/LogManager.hpp"
#include "Platform/Platform.hpp"

#include <Windows.h>

void GuardedMain()
{
}

int32 LaunchEngine()
{
    FLogManager::Initialize();
    FPlatform::Initialize();
    TRY
    {
        GuardedMain();
    }
    CATCH()
    {
    }
    FPlatform::Shutdown();
    FLogManager::Shutdown();
    return FExceptionHandler::GetExitCode();
}
