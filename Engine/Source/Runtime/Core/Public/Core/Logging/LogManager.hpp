// RavenStorm Copyright @ 2025-2025

#pragma once

#include "LogChannel.hpp"

#include "Core/Containers/String.hpp"
#include "Core/Utility/StringUtils.hpp"

class CORE_API FLogManager
{
public:
    static void Initialize();
    static void Shutdown();

    static void Log(const FLogChannel& Channel, ELogSeverity Severity, const FAnsiString& Message);
    static void Log(const FLogChannel& Channel, ELogSeverity Severity, const FWideString& Message);

    template <typename... TArguments>
    static void Log(const FLogChannel& Channel, const ELogSeverity Severity, TAnsiFormatString<TArguments...> Format, TArguments&&... Arguments)
    {
        Log(Channel, Severity, StringUtils::Format<TArguments...>(Format, std::forward<TArguments>(Arguments)...));
    }

    template <typename... TArguments>
    static void Log(const FLogChannel& Channel, const ELogSeverity Severity, TWideFormatString<TArguments...> Format, TArguments&&... Arguments)
    {
        Log(Channel, Severity, StringUtils::FormatWide<TArguments...>(Format, std::forward<TArguments>(Arguments)...));
    }
};

#define CVLOG(Channel, Severity, Message, ...) FLogManager::Log(Channel, ELogSeverity::Severity, Message __VA_OPT__(,) __VA_ARGS__)

CORE_API DECLARE_LOG_CHANNEL_EXTERN(Temp)
