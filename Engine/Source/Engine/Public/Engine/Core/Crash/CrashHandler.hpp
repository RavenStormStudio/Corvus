#pragma once

#include <optional>
#include <type_traits>
#include <Windows.h>

#include "Engine/Core/CoreTypes.hpp"

class ENGINE_API FCrashHandler
{
public:
    static void Setup();

    static LONG Handler(EXCEPTION_POINTERS* ExceptionPointers);

    [[nodiscard]] static int32 GetExitCode();

private:

private:
    static int32 ExitCode;
};

template <typename TFunction, typename TReturnType = std::invoke_result_t<TFunction>> requires !std::is_same_v<void, std::invoke_result_t<TFunction>> && std::is_default_constructible_v<TReturnType>
TReturnType GuardedExecute(TFunction&& Function, TReturnType InvalidValue = TReturnType())
{
    __try
    {
        if constexpr (std::is_void_v<decltype(Function())>)
        {
            std::forward<TFunction>(Function)();
            return;
        }
        else
        {
            return std::forward<TFunction>(Function)();
        }
    }
    __except (FCrashHandler::Handler(GetExceptionInformation()))
    {
        return InvalidValue;
    }
}

template <typename TFunction> requires std::is_same_v<void, std::invoke_result_t<TFunction>>
void GuardedExecute(TFunction&& Function)
{
    __try
    {
        std::forward<TFunction>(Function)();
    }
    __except (FCrashHandler::Handler(GetExceptionInformation()))
    {
    }
}

#define TRY __try
#define CATCH() __except(FCrashHandler::Handler(GetExceptionInformation()))
#define CATCH_HANDLER(Handler) __except(Handler(GetExceptionInformation()))
