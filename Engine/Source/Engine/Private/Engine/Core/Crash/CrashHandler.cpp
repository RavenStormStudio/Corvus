#include "Engine/Core/Crash/CrashHandler.hpp"

#include <Windows.h>

#include "Engine/Core/Crash/Stackwalker.hpp"

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
    MessageBoxA(nullptr, FStackWalker::FormatStackTrace(StackFrames).c_str(), TEXT("Corvus"), MB_ICONERROR | MB_OK);
    return EXCEPTION_EXECUTE_HANDLER;
}

int32 FCrashHandler::GetExitCode()
{
    return ExitCode;
}
