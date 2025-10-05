// RavenStorm Copyright @ 2025-2025

#include "Core/Assertion/Assertion.hpp"

#include <type_traits>
#include <Windows.h>

#include "Core/Assertion/ExceptionHandler.hpp"
#include "Core/Logging/LogManager.hpp"

DEFINE_LOG_CHANNEL(Assert, All)

void Assertion::Assert(FString&& Condition, FString&& Message, const long CustomCode, const FSourceLocation& Location)
{
#ifdef CORVUS_MODE_DEBUG
    if (IsDebuggerPresent())
    {
        __debugbreak();
    }
#endif
    FExceptionMetadata Metadata;
    Metadata.ReportCode = ERROR_CODE_ASSERTION;
    Metadata.AssertionCondition = std::move(Condition);
    Metadata.Message = std::move(Message);
    Metadata.SourceLocation = Location;
    Metadata.CustomCode = CustomCode;
    FExceptionHandler::Report(Metadata);
}

void Assertion::Assert(const FExceptionMetadata& Metadata)
{
#ifdef CORVUS_MODE_DEBUG
    if (IsDebuggerPresent())
    {
        __debugbreak();
    }
#endif
    FExceptionHandler::Report(Metadata);
}
