// RavenStorm Copyright @ 2025-2025

#pragma once

#include <Windows.h>

#include "Core/Containers/String.hpp"
#include "Core/Utility/SourceLocation.hpp"

#define ERROR_CODE_UNKNOWN MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0001)
#define ERROR_CODE_ASSERTION MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x0002)

struct CORE_API FExceptionMetadata
{
    HRESULT ReportCode = ERROR_CODE_UNKNOWN;
    FString AssertionCondition; // Only valid if report code is ERROR_CODE_ASSERTION
    FString Message;
    HRESULT CustomCode = 0;
    FSourceLocation SourceLocation;
};

class CORE_API FExceptionHandler
{
public:
    [[nodiscard]] static long HandleException(const EXCEPTION_POINTERS* ExceptionInfo);
    static void Report(const FExceptionMetadata& Metadata);
    [[nodiscard]] static FString GetResultDescription(HRESULT Result);

    [[nodiscard]] static int32 GetExitCode();

private:
    static int32 ExitCode;
};

[[nodiscard]] CORE_API long HandleException(const EXCEPTION_POINTERS* ExceptionInfo);

#define TRY __try
#define CATCH() __except(HandleException(GetExceptionInformation()))
