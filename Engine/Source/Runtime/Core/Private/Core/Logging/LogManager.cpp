// RavenStorm Copyright @ 2025-2025

#include "Core/Logging/LogManager.hpp"

#include <spdlog/async.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

static constexpr const FAnsiChar* LoggerPattern = "%^[%T] [%t] [%-23!n] %8l:%$ %v";

void FLogManager::Initialize()
{
    spdlog::init_thread_pool(8192, 1);
    spdlog::set_level(static_cast<spdlog::level::level_enum>(ELogSeverity::All));
    spdlog::set_pattern(LoggerPattern);
}

void FLogManager::Shutdown()
{
    spdlog::shutdown();
}

void FLogManager::Log(const FLogChannel& Channel, ELogSeverity Severity, const FString& Message)
{
    if (Severity < Channel.Severity)
    {
        return;
    }
    std::shared_ptr<spdlog::logger> Logger = spdlog::get(Channel.Name);
    if (Logger == nullptr)
    {
        spdlog::sinks_init_list InitializerList = {
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
        };
        Logger = std::make_shared<spdlog::async_logger>(Channel.Name, InitializerList, spdlog::thread_pool());
        Logger->set_pattern(LoggerPattern);
        Logger->set_level(static_cast<spdlog::level::level_enum>(Channel.Severity));
        spdlog::register_logger(Logger);
    }
    Logger->log(static_cast<spdlog::level::level_enum>(Severity), Message);
}

void FLogManager::Log(const FLogChannel& Channel, ELogSeverity Severity, const FWideString& Message)
{
    if (Severity < Channel.Severity)
    {
        return;
    }
    std::shared_ptr<spdlog::logger> Logger = spdlog::get(Channel.Name);
    if (Logger == nullptr)
    {
        spdlog::sinks_init_list InitializerList = {
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>(),
        };
        Logger = std::make_shared<spdlog::async_logger>(Channel.Name, InitializerList, spdlog::thread_pool());
        Logger->set_pattern(LoggerPattern);
        Logger->set_level(static_cast<spdlog::level::level_enum>(Channel.Severity));
        spdlog::register_logger(Logger);
    }
    Logger->log(static_cast<spdlog::level::level_enum>(Severity), Message);
}
