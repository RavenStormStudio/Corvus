// RavenStorm Copyright @ 2025-2025

#pragma once

#ifdef CORVUS_BUILD_MODULAR
#   ifdef CORVUS_BUILD_LAUNCH
#       define LAUNCH_API __declspec(dllexport)
#   else
#       define LAUNCH_API __declspec(dllimport)
#   endif
#else
#   define LAUNCH_API
#endif
