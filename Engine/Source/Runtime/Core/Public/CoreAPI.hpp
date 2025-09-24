// RavenStorm Copyright @ 2025-2025

#pragma once

#ifdef CORVUS_BUILD_MODULAR
#   ifdef CORVUS_BUILD_CORE
#       define CORE_API __declspec(dllexport)
#   else
#       define CORE_API __declspec(dllimport)
#   endif
#else
#   define CORE_API
#endif
