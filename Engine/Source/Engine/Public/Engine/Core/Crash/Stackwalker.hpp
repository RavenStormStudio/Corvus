#pragma once

#include "Engine/Core/CoreDefinitions.hpp"
#include "Engine/Core/Containers/String.hpp"
#include "Engine/Core/Containers/Vector.hpp"

#include <Windows.h>

struct ENGINE_API FStackFrame
{
    FString ModuleName;
    FString FunctionName;
    FString FileName;
    uint32 LineNumber;
    uint64 Address;
    uint64 ModuleBase;
    uint64 Offset;

    [[nodiscard]] FString ToString() const;
};

struct ENGINE_API FStackWalkerConfig
{
    uint32 MaxFrames = 64;
    uint32 SkipFrames = 0;
    bool8 bCaptureSourceInfo = true;
    bool8 bUseSymbols = true;
    bool8 bUndecorateSymbols = true;
    FString SymbolSearchPath;
};

class ENGINE_API FStackWalker
{
public:
    FStackWalker(FStackWalkerConfig InConfig = {});
    ~FStackWalker();

    NON_COPYABLE(FStackWalker,)
    DEFAULT_MOVEABLE(FStackWalker,)

public:
    [[nodiscard]] TVector<FStackFrame> CaptureStack() const;
    [[nodiscard]] TVector<FStackFrame> CaptureStack(HANDLE ThreadHandle) const;
    [[nodiscard]] TVector<FStackFrame> CaptureStack(CONTEXT* Context) const;

public:
    static FString FormatStackTrace(const TVector<FStackFrame>& Frames);

private:
    bool8 InitializeSymbols() const;
    void CleanupSymbols() const;

    TVector<FStackFrame> WalkStack(CONTEXT* Context, HANDLE ThreadHandle) const;
    bool8 ResolveSymbol(uint64 Address, FStackFrame& OutFrame) const;

private:
    static FString GetModuleName(uint64 Address);

private:
    FStackWalkerConfig Config;
    HANDLE ProcessHandle;
};
