// RavenStorm Copyright @ 2025-2025

#include "Core/Assertion/ExceptionHandler.hpp"

#include <cstdlib>
#include <Windows.h>

#include "Core/Logging/LogManager.hpp"

int32 FExceptionHandler::ExitCode = EXIT_SUCCESS;

FString FormatSourceLocation(const FSourceLocation& SourceLocation)
{
    FString Result;
    FString FileName = SourceLocation.file_name();
    const size64 LastSlashIndex = FileName.find_last_of('\\');
    if (LastSlashIndex != FString::npos)
    {
        FileName = FileName.substr(LastSlashIndex + 1);
    }
    Result += "File: " + FileName + "\n";
    Result += "Line: " + std::to_string(SourceLocation.line()) + "\n";
    Result += "Function: " + FString(SourceLocation.function_name()) + "\n";
    return Result;
}

long FExceptionHandler::HandleException(const EXCEPTION_POINTERS* ExceptionInfo)
{
    ExitCode = EXIT_FAILURE;

    const HRESULT ExceptionCode = static_cast<HRESULT>(ExceptionInfo->ExceptionRecord->ExceptionCode);

    FString MessageBoxCaption;
    FString MessageBoxText;
    uint32 MessageBoxType;
    if (ExceptionCode == ERROR_CODE_ASSERTION)
    {
        const FExceptionMetadata* Metadata = reinterpret_cast<FExceptionMetadata*>(ExceptionInfo->ExceptionRecord->ExceptionInformation[0]);

        MessageBoxCaption = "Corvus | Assertion";
        MessageBoxText = "Assertion Failure!\n\n";
        if (Metadata->Message.empty() && Metadata->CustomCode != 0)
        {
            MessageBoxText += GetResultDescription(Metadata->CustomCode) + "\n\n";
        }
        else
        {
            MessageBoxText += Metadata->Message + "\n\n";
        }
        MessageBoxText += "Condition: " + Metadata->AssertionCondition + "\n";
        MessageBoxText += FormatSourceLocation(Metadata->SourceLocation);
        if (!Metadata->Message.empty() && Metadata->CustomCode != 0)
        {
            MessageBoxText += "Result: " + GetResultDescription(Metadata->CustomCode) + "\n";
        }
        MessageBoxType = MB_OK | MB_ICONERROR | MB_TASKMODAL | MB_SETFOREGROUND | MB_TOPMOST;
    }
    else
    {
        MessageBoxCaption = "Corvus | Exception";
        MessageBoxText = "Unhandled Exception!\n\n";
        MessageBoxText += "Result: " + GetResultDescription(ExceptionCode) + "\n";
        MessageBoxType = MB_OK | MB_ICONERROR | MB_TASKMODAL | MB_SETFOREGROUND | MB_TOPMOST;
    }
    MessageBoxEx(nullptr, MessageBoxText.c_str(), MessageBoxCaption.c_str(), MessageBoxType, MAKELANGID(LANG_NEUTRAL, SUBLANG_ENGLISH_US));
    return EXCEPTION_EXECUTE_HANDLER;
}

void FExceptionHandler::Report(const FExceptionMetadata& Metadata)
{
    ULONG_PTR Parameters[] = {reinterpret_cast<ULONG_PTR>(&Metadata)};
    RaiseException(Metadata.ReportCode, EXCEPTION_NONCONTINUABLE, _countof(Parameters), Parameters);
}

FString FExceptionHandler::GetResultDescription(const HRESULT Result)
{
    if (HRESULT_FACILITY(Result) == FACILITY_ITF)
    {
        return "Unknown";
    }
    LPSTR Buffer = nullptr;
    const size64 Size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, Result,
                                      MAKELANGID(LANG_NEUTRAL, SUBLANG_ENGLISH_US), reinterpret_cast<LPSTR>(&Buffer), 0, nullptr);
    FString Message(Buffer, Size);
    LocalFree(Buffer);
    if (!Message.empty())
    {
        Message = Message.substr(0, Message.size() - 2);
        return Message;
    }
    return {};
}

int32 FExceptionHandler::GetExitCode()
{
    return ExitCode;
}

long HandleException(const EXCEPTION_POINTERS* ExceptionInfo)
{
    return FExceptionHandler::HandleException(ExceptionInfo);
}
