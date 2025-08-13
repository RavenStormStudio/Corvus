#include "Engine/Core/Crash/Stackwalker.hpp"

#include <DbgHelp.h>

static constexpr size64 MaxSymbolNameLength = 256;
static constexpr size64 SymbolBufferSize = sizeof(SYMBOL_INFO) + MaxSymbolNameLength * sizeof(FChar);

struct FDbgHelpFunctions
{
    decltype(&::SymInitialize) SymInitialize = nullptr;
    decltype(&::SymCleanup) SymCleanup = nullptr;
    decltype(&::SymSetOptions) SymSetOptions = nullptr;
    decltype(&::SymFromAddr) SymFromAddr = nullptr;
    decltype(&::SymGetLineFromAddr64) SymGetLineFromAddr64 = nullptr;
    decltype(&::StackWalk64) StackWalk64 = nullptr;
    decltype(&::SymFunctionTableAccess64) SymFunctionTableAccess64 = nullptr;
    decltype(&::SymGetModuleBase64) SymGetModuleBase64 = nullptr;
    decltype(&::SymGetModuleInfo64) SymGetModuleInfo64 = nullptr;
    decltype(&::UnDecorateSymbolName) UnDecorateSymbolName = nullptr;

    bool8 Load()
    {
        if (DbgHelpModule)
        {
            return true;
        }

        WCHAR SystemPath[MAX_PATH];
        if (GetSystemDirectoryW(SystemPath, MAX_PATH))
        {
            const FWideString DbgHelpPath = FWideString(SystemPath) + L"\\dbghelp.dll";
            DbgHelpModule = LoadLibraryW(DbgHelpPath.c_str());
        }
        if (!DbgHelpModule)
        {
            DbgHelpModule = LoadLibraryW(L"dbghelp.dll");
        }
        if (!DbgHelpModule)
        {
            return false;
        }

        bool bSuccess = true;
        bSuccess &= LoadFunction(SymInitialize, "SymInitialize");
        bSuccess &= LoadFunction(SymCleanup, "SymCleanup");
        bSuccess &= LoadFunction(SymSetOptions, "SymSetOptions");
        bSuccess &= LoadFunction(SymFromAddr, "SymFromAddr");
        bSuccess &= LoadFunction(SymGetLineFromAddr64, "SymGetLineFromAddr64");
        bSuccess &= LoadFunction(StackWalk64, "StackWalk64");
        bSuccess &= LoadFunction(SymFunctionTableAccess64, "SymFunctionTableAccess64");
        bSuccess &= LoadFunction(SymGetModuleBase64, "SymGetModuleBase64");
        bSuccess &= LoadFunction(SymGetModuleInfo64, "SymGetModuleInfo64");
        bSuccess &= LoadFunction(UnDecorateSymbolName, "UnDecorateSymbolName");

        if (!bSuccess)
        {
            FreeLibrary(DbgHelpModule);
            DbgHelpModule = nullptr;
            return false;
        }
        return true;
    }

    void Unload()
    {
        if (DbgHelpModule)
        {
            FreeLibrary(DbgHelpModule);
            DbgHelpModule = nullptr;
        }
    }

public:
    static FDbgHelpFunctions& GetInstance()
    {
        static FDbgHelpFunctions DbgHelpFunctions;
        return DbgHelpFunctions;
    }

private:
    template <typename T>
    bool LoadFunction(T& OutFunc, const char* FunctionName)
    {
        void* FuncPtr = reinterpret_cast<void*>(GetProcAddress(DbgHelpModule, FunctionName));
        if (!FuncPtr)
        {
            return false;
        }
        OutFunc = reinterpret_cast<T>(FuncPtr);
        return true;
    }

private:
    HMODULE DbgHelpModule = nullptr;
};

FString FStackFrame::ToString() const
{
    if (!FileName.empty() && LineNumber > 0)
    {
        return String::Format(TEXT("[{}] {} at {}:{} (0x{:016X})"), ModuleName, FunctionName, FileName, LineNumber, Address);
    }
    if (!FunctionName.empty())
    {
        return String::Format(TEXT("[{}] {} + 0x{:X} (0x{:016X})"), ModuleName, FunctionName, Offset, Address);
    }
    return String::Format(TEXT("[{}] 0x{:016X}"), ModuleName, Address);
}

FStackWalker::FStackWalker(FStackWalkerConfig InConfig)
    : Config(std::move(InConfig))
{
    ProcessHandle = GetCurrentProcess();
}

FStackWalker::~FStackWalker() = default;

TVector<FStackFrame> FStackWalker::CaptureStack() const
{
    CONTEXT Context;
    RtlCaptureContext(&Context);
    return CaptureStack(&Context);
}

TVector<FStackFrame> FStackWalker::CaptureStack(const HANDLE ThreadHandle) const
{
    CONTEXT Context = {};
    Context.ContextFlags = CONTEXT_FULL;
    if (ThreadHandle == GetCurrentThread())
    {
        RtlCaptureContext(&Context);
    }
    else
    {
        SuspendThread(ThreadHandle);
        GetThreadContext(ThreadHandle, &Context);
        ResumeThread(ThreadHandle);
    }
    return WalkStack(&Context, ThreadHandle);
}

TVector<FStackFrame> FStackWalker::CaptureStack(CONTEXT* Context) const
{
    return WalkStack(Context, GetCurrentThread());
}

FString FStackWalker::FormatStackTrace(const TVector<FStackFrame>& Frames)
{
    FStringStream Stream;
    Stream << TEXT("Stack Trace:\n");
    for (size64 Index = 0; Index < Frames.size(); ++Index)
    {
        Stream << std::format(TEXT("  [{}] {}"), Index, Frames[Index].ToString()) << TEXT("\n");
    }
    return Stream.str();
}

bool8 FStackWalker::InitializeSymbols() const
{
    if (!FDbgHelpFunctions::GetInstance().Load())
    {
        return false;
    }

    DWORD SymOptions = SYMOPT_DEFERRED_LOADS | SYMOPT_FAIL_CRITICAL_ERRORS;
    if (Config.bUndecorateSymbols)
    {
        SymOptions |= SYMOPT_UNDNAME;
    }
    if (Config.bCaptureSourceInfo)
    {
        SymOptions |= SYMOPT_LOAD_LINES;
    }
    FDbgHelpFunctions::GetInstance().SymSetOptions(SymOptions);

    const char* SymbolSearchPath = Config.SymbolSearchPath.empty() ? nullptr : Config.SymbolSearchPath.c_str();
    if (FDbgHelpFunctions::GetInstance().SymInitialize(ProcessHandle, SymbolSearchPath, true))
    {
        return true;
    }
    return false;
}

void FStackWalker::CleanupSymbols() const
{
    FDbgHelpFunctions::GetInstance().SymCleanup(ProcessHandle);
    FDbgHelpFunctions::GetInstance().Unload();
}

TVector<FStackFrame> FStackWalker::WalkStack(CONTEXT* Context, const HANDLE ThreadHandle) const
{
    if (!InitializeSymbols())
    {
        return {};
    }

    TVector<FStackFrame> StackFrames;
    StackFrames.reserve(Config.MaxFrames);

    STACKFRAME64 StackFrame = {};
    DWORD MachineType;

#ifdef _M_X64
    MachineType = IMAGE_FILE_MACHINE_AMD64;
    StackFrame.AddrPC.Offset = Context->Rip;
    StackFrame.AddrPC.Mode = AddrModeFlat;
    StackFrame.AddrFrame.Offset = Context->Rbp;
    StackFrame.AddrFrame.Mode = AddrModeFlat;
    StackFrame.AddrStack.Offset = Context->Rsp;
    StackFrame.AddrStack.Mode = AddrModeFlat;
#elif _M_IX86
    MachineType = IMAGE_FILE_MACHINE_I386;
    StackFrame.AddrPC.Offset = Context->Eip;
    StackFrame.AddrPC.Mode = AddrModeFlat;
    StackFrame.AddrFrame.Offset = Context->Ebp;
    StackFrame.AddrFrame.Mode = AddrModeFlat;
    StackFrame.AddrStack.Offset = Context->Esp;
    StackFrame.AddrStack.Mode = AddrModeFlat;
#else
#   error "Unsupported platform"
#endif

    uint32 FrameCount = 0;
    uint32 SkippedFrames = 0;

    while (FrameCount < Config.MaxFrames)
    {
        if (!FDbgHelpFunctions::GetInstance().StackWalk64(MachineType, ProcessHandle, ThreadHandle, &StackFrame, Context, nullptr, FDbgHelpFunctions::GetInstance().SymFunctionTableAccess64,
                                                          FDbgHelpFunctions::GetInstance().SymGetModuleBase64, nullptr))
        {
            break;
        }
        if (StackFrame.AddrPC.Offset == 0)
        {
            break;
        }
        if (SkippedFrames < Config.SkipFrames)
        {
            SkippedFrames++;
            continue;
        }
        FStackFrame Frame;
        Frame.Address = StackFrame.AddrPC.Offset;
        if (ResolveSymbol(Frame.Address, Frame))
        {
            StackFrames.push_back(std::move(Frame));
            FrameCount++;
        }
    }
    CleanupSymbols();
    return StackFrames;
}

bool8 FStackWalker::ResolveSymbol(const uint64 Address, FStackFrame& OutFrame) const
{
    OutFrame.Address = Address;
    OutFrame.ModuleName = GetModuleName(Address);
    if (!Config.bUseSymbols)
    {
        return true;
    }
    uint8 SymbolBuffer[SymbolBufferSize];
    const PSYMBOL_INFO Symbol = reinterpret_cast<PSYMBOL_INFO>(SymbolBuffer);
    Symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    Symbol->MaxNameLen = MaxSymbolNameLength;

    DWORD64 Displacement = 0;
    if (FDbgHelpFunctions::GetInstance().SymFromAddr(ProcessHandle, Address, &Displacement, Symbol))
    {
        OutFrame.FunctionName = Symbol->Name;
        OutFrame.Offset = Displacement;
        OutFrame.ModuleBase = Symbol->ModBase;
    }

    if (Config.bCaptureSourceInfo)
    {
        IMAGEHLP_LINE64 Line = {};
        Line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        DWORD LineDisplacement = 0;
        if (FDbgHelpFunctions::GetInstance().SymGetLineFromAddr64(ProcessHandle, Address, &LineDisplacement, &Line))
        {
            OutFrame.FileName = Line.FileName;
            if (OutFrame.FileName.find_last_of('\\') != FString::npos)
            {
                OutFrame.FileName = OutFrame.FileName.substr(OutFrame.FileName.find_last_of('\\') + 1);
            }
            OutFrame.LineNumber = Line.LineNumber;
        }
    }
    return true;
}

FString FStackWalker::GetModuleName(const uint64 Address)
{
    HMODULE Module = nullptr;
    if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, reinterpret_cast<LPCTSTR>(Address), &Module))
    {
        char ModuleName[MAX_PATH];
        if (GetModuleFileNameA(Module, ModuleName, sizeof(ModuleName)))
        {
            FString FullPath = ModuleName;
            const size_t LastSlash = FullPath.find_last_of("\\/");
            if (LastSlash != FString::npos)
            {
                return FullPath.substr(LastSlash + 1);
            }
            return FullPath;
        }
    }
    return TEXT("Unknown");
}
