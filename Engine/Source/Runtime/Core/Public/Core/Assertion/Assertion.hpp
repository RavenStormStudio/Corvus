// RavenStorm Copyright @ 2025-2025

#pragma once

#include "Core/Containers/String.hpp"
#include "Core/Logging/LogChannel.hpp"
#include "Core/Utility/SourceLocation.hpp"
#include "Core/Utility/StringUtils.hpp"

struct FExceptionMetadata;
CORE_API DECLARE_LOG_CHANNEL_EXTERN(Assert)

namespace Assertion
{
    CORE_API void Assert(FString&& Condition, FString&& Message = "", long CustomCode = 0, const FSourceLocation& Location = FSourceLocation::current());
    CORE_API void Assert(const FExceptionMetadata& Metadata);
}

#define ASSERT_IF(Condition, ...) \
    do \
    { \
        if(!(Condition)) \
        { \
            Assertion::Assert(FExceptionMetadata{ .AssertionCondition = FString(TEXT(#Condition)) __VA_OPT__(, .Message = StringUtils::Format(__VA_ARGS__)), .SourceLocation = FSourceLocation::current() }); \
        } \
    } while (false)

#define ASSERT_RESULT(Result, ...) \
    do \
    { \
        if(FAILED(Result)) \
        { \
            Assertion::Assert(FExceptionMetadata{ .ReportCode = ERROR_CODE_ASSERTION, .AssertionCondition = FString(TEXT(#Result)) __VA_OPT__(, .Message = StringUtils::Format(__VA_ARGS__)), .CustomCode = Result, .SourceLocation = FSourceLocation::current() }); \
        } \
    } while (false)
