#pragma once

#include "Engine/Core/Containers/String.hpp"
#include "Engine/Core/Utility/SourceLocation.hpp"

#include <Windows.h>

struct ENGINE_API FAssertionData
{
    HRESULT ResultCode;
    FString Condition;
    FString Message;
    FSourceLocation SourceLocation;

    FAssertionData(const FString& InCondition, HRESULT InResultCode = 0, const FString& InMessage = FString(), const FSourceLocation& InSourceLocation = FSourceLocation::current());
};

ENGINE_API void OnAssertion(const FAssertionData& AssertionData);

#define VERIFY(Condition, ...) \
    do \
    { \
        if (!(Condition)) \
        { \
            OnAssertion(FAssertionData(#Condition, 0 __VA_OPT__(, String::Format(__VA_ARGS__)))); \
        } \
    } while(false)

#define VERIFY_RESULT(Result, ...) \
    do \
    { \
        HRESULT ResultCode = (Result); \
        if (FAILED(ResultCode)) \
        { \
            OnAssertion(FAssertionData(#Result, ResultCode __VA_OPT__(, String::Format(__VA_ARGS__)))); \
        } \
    } while(false)


#if defined(CORVUS_MODE_DEBUG) || defined(CORVUS_MODE_DEVELOPMENT)
#   define ASSERT(Condition, ...) VERIFY(Condition, __VA_ARGS__)
#   define ASSERT_RESULT(Result, ...) VERIFY_RESULT(Result, __VA_ARGS__)
#else
#   define ASSERT(Condition, ...)
#   define ASSERT_RESULT(Result, ...)
#endif
