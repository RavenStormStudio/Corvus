// RavenStorm Copyright @ 2025-2025

#pragma once

#include "LogSeverity.hpp"
#include "Core/Containers/String.hpp"

struct CORE_API FLogChannel
{
    FAnsiString Name;
    ELogSeverity Severity;
};

#define DECLARE_LOG_CHANNEL_EXTERN(ChannelName) \
    extern FLogChannel Log##ChannelName##;

#define DECLARE_LOG_CHANNEL(ChannelName, DefaultSeverity) \
    FLogChannel Log##ChannelName## = { .Name = ("Log"#ChannelName), .Severity = ELogSeverity::DefaultSeverity };

#define DEFINE_LOG_CHANNEL(ChannelName, DefaultSeverity) \
    DECLARE_LOG_CHANNEL(ChannelName, DefaultSeverity);
