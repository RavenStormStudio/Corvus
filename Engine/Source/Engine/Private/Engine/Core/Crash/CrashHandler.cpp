#include "Engine/Core/Crash/CrashHandler.hpp"

#include <Windows.h>

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

    const FStackWalker StackWalker;
    const TVector<FStackFrame> StackFrames = StackWalker.CaptureStack();

    CVLOG(LogCrashHandler, Critical, "Stack Trace:");
    for (size64 Index = 0; Index < StackFrames.size(); ++Index)
    {
        CVLOG(LogCrashHandler, Critical, "\t[{}] {}", Index, StackFrames[Index].ToString());
    }

    MessageBoxA(nullptr, FStackWalker::FormatStackTrace(StackFrames).c_str(), TEXT("Corvus"), MB_ICONERROR | MB_OK);
    return EXCEPTION_EXECUTE_HANDLER;
}

int32 FCrashHandler::GetExitCode()
{
    return ExitCode;
}
