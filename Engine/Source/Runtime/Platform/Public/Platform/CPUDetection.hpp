// RavenStorm Copyright @ 2025-2025

#pragma once

#include "Core/Containers/String.hpp"

struct PLATFORM_API FCPUInfo
{
    FAnsiString VendorString;
    FAnsiString BrandString;
    int32 Family;
    int32 Model;
    int32 Stepping;
    int32 PhysicalCores;
    int32 LogicalCores;
    int32 CacheLine;
    int32 L1CacheSize;
    int32 L2CacheSize;
    int32 L3CacheSize;
    bool8 IsIntel;
    bool8 IsAMD;
    bool8 IsHyperThreaded;
};

class PLATFORM_API FCPUDetection
{
private:
    static void Initialize();

    [[nodiscard]] static FCPUInfo GetCPUInfo() noexcept;

private:
    static void DetectVendor();
    static void DetectBrand();
    static void DetectCPUTopology();

private:
    static FCPUInfo CPUInfo;

    friend class PLATFORM_API FPlatform;
};
