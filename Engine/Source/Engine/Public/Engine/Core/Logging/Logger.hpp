#pragma once

#include "Engine/Core/Containers/Name.hpp"
#include "Engine/Core/Containers/String.hpp"

#include <type_traits>

enum class ENGINE_API ELogSeverity : uint8
{
    Trace = 0,
    Debug = 1,
    Info = 2,
    Warn = 3,
    Error = 4,
    Critical = 5,
    All = 0,
    Off = 6,
};

struct ENGINE_API FLogChannel
{
    FName ChannelName;
    ELogSeverity Severity;
};

class ENGINE_API FLogger
{
public:
    static void Setup();
    static void Destroy();

    static void Log(const FLogChannel& Channel, ELogSeverity Severity, const FString& Message);

    template <typename... TArguments>
    static void Log(const FLogChannel& Channel, const ELogSeverity Severity, TFormatString<TArguments...> Format, TArguments&&... Arguments)
    {
        Log(Channel, Severity, String::Format<TArguments...>(Format, std::forward<TArguments>(Arguments)...));
    }
};

#define DEFINE_LOG_CHANNEL_EXTERN(Name) \
    extern FLogChannel Log##Name##;

#define DECLARE_LOG_CHANNEL(Name, DefaultSeverity) \
    FLogChannel Log##Name## = { .ChannelName = #Name, .Severity = ELogSeverity::DefaultSeverity };

#define CVLOG(Channel, Severity, Message, ...) FLogger::Log(Channel, ELogSeverity::Severity, Message, __VA_ARGS__)

ENGINE_API DEFINE_LOG_CHANNEL_EXTERN(Temp)
