// RavenStorm Copyright @ 2025-2025

#pragma once

#include "Core/Containers/String.hpp"
#include "Core/Utility/SourceLocation.hpp"
#include "Core/Utility/WindowsDefinitions.hpp"

#define ERROR_CODE_UNKNOWN CREATE_HRESULT(1, 4, 0x0001)
#define ERROR_CODE_ASSERTION CREATE_HRESULT(1, 4, 0x0002)

struct CORE_API FExceptionMetadata
{
    Windows::HRESULT ReportCode = ERROR_CODE_UNKNOWN;
    FString AssertionCondition; // Only valid if report code is ERROR_CODE_ASSERTION
    FString Message;
    Windows::HRESULT CustomCode = 0;
    FSourceLocation SourceLocation;
};

class CORE_API FExceptionHandler
{
public:
    [[nodiscard]] static long HandleException(const Windows::EXCEPTION_POINTERS* ExceptionInfo);
    static void Report(const FExceptionMetadata& Metadata);
    [[nodiscard]] static FString GetResultDescription(Windows::HRESULT Result);

    [[nodiscard]] static int32 GetExitCode();

private:
    static int32 ExitCode;
};

[[nodiscard]] CORE_API long HandleException(const Windows::EXCEPTION_POINTERS* ExceptionInfo);

#define TRY __try
#define CATCH() __except(HandleException(GetExceptionInformation()))
