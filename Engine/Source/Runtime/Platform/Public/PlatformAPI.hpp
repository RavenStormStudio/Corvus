// RavenStorm Copyright @ 2025-2025

#pragma once

#ifdef CORVUS_BUILD_MODULAR
#   ifdef CORVUS_BUILD_PLATFORM
#       define PLATFORM_API __declspec(dllexport)
#   else
#       define PLATFORM_API __declspec(dllimport)
#   endif
#else
#   define PLATFORM_API
#endif
