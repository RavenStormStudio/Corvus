// RavenStorm Copyright @ 2025-2025

#pragma once

#if defined(CORVUS_MODE_DEBUG) || defined(CORVUS_MODE_DEVELOPMENT)
#   define ENTRY_POINT_SIGNATURE() int main()
#else
#include <Windows.h>

#   define ENTRY_POINT_SIGNATURE() int APIENTRY WinMain(HINSTANCE Instance, HINSTANCE PreviousInstance, PSTR CommandLine, int ShowCommand)
#endif

LAUNCH_API int32 LaunchEngine();

#define LAUNCH_ENGINE() \
    ENTRY_POINT_SIGNATURE() \
    { \
        return LaunchEngine(); \
    }
