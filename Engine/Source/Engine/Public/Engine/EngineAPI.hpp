#pragma once

#ifdef CORVUS_BUILD_MODULAR
#   ifdef CORVUS_BUILD_ENGINE
#       define ENGINE_API __declspec(dllexport)
#else
#       define ENGINE_API __declspec(dllimport)
#   endif
#elif defined(CORVUS_BUILD_MONOLITHIC)
#   define ENGINE_API
#else
#   error "Invalid build target specified"
#endif
