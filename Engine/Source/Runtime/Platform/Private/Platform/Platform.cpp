// RavenStorm Copyright @ 2025-2025

#include "Platform/Platform.hpp"
#include "Core/Logging/LogManager.hpp"
#include "Platform/CPUDetection.hpp"

#include <Windows.h>

#include <VersionHelpers.h>

DEFINE_LOG_CHANNEL(Platform, All)

FSystemInfo FPlatform::SystemInfo = {};

bool8 FDynamicModuleHandle::IsValid() const noexcept
{
    return Module != nullptr;
}

void FPlatform::Initialize()
{
    DetectOperatingSystem();
    DetectComputerInfo();

    CVLOG(LogPlatform, Info, "OS: {} ({})", SystemInfo.OSName, SystemInfo.OSVersion);
    CVLOG(LogPlatform, Info, "\t- Build: {}", SystemInfo.OSBuildNumber);
    CVLOG(LogPlatform, Info, "\t- Computer Name: {}", SystemInfo.ComputerName);

    FCPUDetection::Initialize();
}

void FPlatform::Shutdown()
{
}

bool8 FPlatform::HasEnvVariable(const FString& VariableName) noexcept
{
    return GetEnvironmentVariable(VariableName.c_str(), nullptr, 0) > 0;
}

FString FPlatform::GetEnvVariable(const FString& VariableName) noexcept
{
    const DWORD BufferSize = GetEnvironmentVariable(VariableName.c_str(), nullptr, 0);
    if (BufferSize == 0)
    {
        return {};
    }
    FString Result(BufferSize, '\0');
    GetEnvironmentVariable(VariableName.c_str(), Result.data(), BufferSize);
    return Result;
}

FString FPlatform::GetEnvVariable(const FString& VariableName, const FString& DefaultValue) noexcept
{
    FString Value = GetEnvVariable(VariableName);
    return Value.empty() ? DefaultValue : Value;
}

bool8 FPlatform::SetEnvVariable(const FString& VariableName, const FString& Value) noexcept
{
    return SetEnvironmentVariable(VariableName.c_str(), Value.c_str()) != 0;
}

FDynamicModuleHandle FPlatform::LoadDynamicModule(const FString& Name) noexcept
{
    const HMODULE Module = LoadLibrary(Name.c_str());
    if (Module == nullptr)
    {
        CVLOG(LogPlatform, Error, "Failed to load module: {}", Name);
        return {};
    }
    return {.Module = Module};
}

void FPlatform::UnloadDynamicModule(FDynamicModuleHandle&& Module) noexcept
{
    if (!Module.IsValid())
    {
        CVLOG(LogPlatform, Warning, "Attempting to unload invalid module");
        return;
    }
    if (!FreeLibrary(std::move(Module).Module))
    {
        CVLOG(LogPlatform, Error, "Failed to unload module");
    }
}

FCPUInfo FPlatform::GetCPUInfo() noexcept
{
    return FCPUDetection::GetCPUInfo();
}

FSystemInfo FPlatform::GetSystemInfo() noexcept
{
    return SystemInfo;
}

void FPlatform::DetectOperatingSystem()
{
    using RtlGetVersionPtr = LONG(WINAPI*)(PRTL_OSVERSIONINFOW);

    RTL_OSVERSIONINFOW VersionInfo = {};
    VersionInfo.dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);

    if (const HMODULE NtDllModule = GetModuleHandleW(L"ntdll.dll"))
    {
        if (const auto RtlGetVersion = reinterpret_cast<RtlGetVersionPtr>(GetProcAddress(NtDllModule, "RtlGetVersion")))
        {
            RtlGetVersion(&VersionInfo);
        }
    }

    const uint32 MajorVersion = VersionInfo.dwMajorVersion;
    const uint32 MinorVersion = VersionInfo.dwMinorVersion;
    const uint32 BuildNumber = VersionInfo.dwBuildNumber;

    SystemInfo.WindowsVersion = DetermineWindowsVersion(MajorVersion, BuildNumber);
    SystemInfo.OSName = GetWindowsVersionString(MajorVersion, BuildNumber);
    SystemInfo.OSVersion = std::format("{}.{}.{}", MajorVersion, MinorVersion, BuildNumber);
    SystemInfo.OSBuildNumber = std::format("{}", BuildNumber);
}

void FPlatform::DetectComputerInfo()
{
    CHAR ComputerNameBuffer[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD ComputerNameSize = MAX_COMPUTERNAME_LENGTH + 1;
    if (GetComputerName(ComputerNameBuffer, &ComputerNameSize))
    {
        SystemInfo.ComputerName = FString(ComputerNameBuffer);
    }
}

EWindowsVersion FPlatform::DetermineWindowsVersion(const uint32 MajorVersion, const uint32 BuildNumber)
{
    if (MajorVersion == 10)
    {
        if (BuildNumber >= 22000)
        {
            return EWindowsVersion::Windows11;
        }
        return EWindowsVersion::Windows10;
    }
    return EWindowsVersion::Unknown;
}

FString FPlatform::GetWindowsVersionString(const uint32 MajorVersion, const uint32 BuildNumber)
{
    if (MajorVersion == 10)
    {
        if (BuildNumber >= 22000)
        {
            return "Windows 11";
        }
        return "Windows 10";
    }
    return "Windows (Unknown Version)";
}
