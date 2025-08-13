#include "Engine/Core/Logging/Logger.hpp"

#include <spdlog/async.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

DECLARE_LOG_CHANNEL(Temp, All)

static constexpr const FChar* LoggerPattern = "%^[%T] [%t] [%l]%$ %v";

void FLogger::Setup()
{
    spdlog::init_thread_pool(8192, 1);
    spdlog::set_level(static_cast<spdlog::level::level_enum>(ELogSeverity::All));
    spdlog::set_pattern(LoggerPattern);
}

void FLogger::Destroy()
{
    spdlog::shutdown();
}

void FLogger::Log(const FLogChannel& Channel, ELogSeverity Severity, const FString& Message)
{
    if (Severity < Channel.Severity)
    {
        return;
    }
    std::shared_ptr<spdlog::logger> Logger = spdlog::get(Channel.ChannelName.GetString());
    if (Logger == nullptr)
    {
        spdlog::sinks_init_list InitializerList = {
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
        };
        Logger = std::make_shared<spdlog::async_logger>(Channel.ChannelName.GetString(), InitializerList, spdlog::thread_pool());
        Logger->set_pattern(LoggerPattern);
        Logger->set_level(static_cast<spdlog::level::level_enum>(Channel.Severity));
        spdlog::register_logger(Logger);
    }
    Logger->log(static_cast<spdlog::level::level_enum>(Severity), Message);
}
