#include "Engine/Core/Crash/CrashHandler.hpp"

#include <Windows.h>

#include "Engine/Core/Crash/Assertion.hpp"
#include "Engine/Core/Crash/Stackwalker.hpp"

DECLARE_LOG_CHANNEL(CrashHandler, Error)

int32 FCrashHandler::ExitCode = EXIT_SUCCESS;

void FCrashHandler::Setup()
{
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
    SetUnhandledExceptionFilter(Handler);
}

// ReSharper disable once CppParameterMayBeConstPtrOrRef
LONG FCrashHandler::Handler(EXCEPTION_POINTERS* ExceptionPointers)
{
    ExitCode = EXIT_FAILURE;

    FString Message;
    const HRESULT ExceptionCode = static_cast<HRESULT>(ExceptionPointers->ExceptionRecord->ExceptionCode);
    if (ExceptionCode == EXCEPTION_ASSERTION)
    {
        const FAssertionData* AssertionData = reinterpret_cast<FAssertionData*>(ExceptionPointers->ExceptionRecord->ExceptionInformation[0]);
        if (!AssertionData->Message.empty())
        {
            Message += AssertionData->Message + "\n\n";
        }
        Message += "Condition: " + AssertionData->Condition + "\n";

        FString File = AssertionData->SourceLocation.file_name();
        if (File.find_last_of('\\') != FString::npos)
        {
            File = File.substr(File.find_last_of('\\') + 1);
        }
        Message += "File: " + File + "\n";
        Message += "Line: " + String::ToString(AssertionData->SourceLocation.line()) + "\n";
        Message += "Function: " + FString(AssertionData->SourceLocation.function_name()) + "\n";
    }

    const FStackWalker StackWalker;
    const TVector<FStackFrame> StackFrames = StackWalker.CaptureStack();

    CVLOG(LogCrashHandler, Critical, "Stack Trace:");
    for (size64 Index = 0; Index < StackFrames.size(); ++Index)
    {
        CVLOG(LogCrashHandler, Critical, "\t[{}] {}", Index, StackFrames[Index].ToString());
    }

    Message += "\n";
    Message += FStackWalker::FormatStackTrace(StackFrames);
    MessageBoxA(nullptr, Message.c_str(), TEXT("Corvus | Exception"), MB_ICONERROR | MB_OK);
    return EXCEPTION_EXECUTE_HANDLER;
}

int32 FCrashHandler::GetExitCode()
{
    return ExitCode;
}
