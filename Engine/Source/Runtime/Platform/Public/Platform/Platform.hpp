// RavenStorm Copyright @ 2025-2025

#pragma once

#include "CPUDetection.hpp"

#include "Window/Window.hpp"

enum class PLATFORM_API EWindowsVersion : uint8
{
    Unknown = 0,
    Windows10,
    Windows11,
};

struct PLATFORM_API FSystemInfo
{
    FString OSName;
    FString OSVersion;
    FString OSBuildNumber;
    EWindowsVersion WindowsVersion;
    FString ComputerName;
};

struct PLATFORM_API FDynamicModuleHandle
{
    Windows::HMODULE Module = nullptr;

public:
    [[nodiscard]] bool8 IsValid() const noexcept;
};

class PLATFORM_API FPlatform
{
public:
    static void Initialize();
    static void Shutdown();

    [[nodiscard]] static bool8 HasEnvVariable(const FString& VariableName) noexcept;
    [[nodiscard]] static FString GetEnvVariable(const FString& VariableName) noexcept;
    [[nodiscard]] static FString GetEnvVariable(const FString& VariableName, const FString& DefaultValue) noexcept;
    static bool8 SetEnvVariable(const FString& VariableName, const FString& Value) noexcept;

    [[nodiscard]] static FDynamicModuleHandle LoadDynamicModule(const FString& Name) noexcept;
    static void UnloadDynamicModule(FDynamicModuleHandle&& Module) noexcept;

    [[nodiscard]] static FCPUInfo GetCPUInfo() noexcept;
    [[nodiscard]] static FSystemInfo GetSystemInfo() noexcept;

private:
    static void DetectOperatingSystem();
    static void DetectComputerInfo();

    static EWindowsVersion DetermineWindowsVersion(uint32 MajorVersion, uint32 BuildNumber);
    static FString GetWindowsVersionString(uint32 MajorVersion, uint32 BuildNumber);

private:
    static FSystemInfo SystemInfo;
};
