// RavenStorm Copyright @ 2025-2025

#include "Platform/CPUDetection.hpp"

#include "Core/Logging/LogChannel.hpp"
#include "Core/Logging/LogManager.hpp"

#include <Windows.h>

#include <utility>

#include "Core/Memory/Memory.hpp"

DEFINE_LOG_CHANNEL(CPUDetection, All)

namespace
{
    struct FCPUIDResult
    {
        int32 EAX;
        int32 EBX;
        int32 ECX;
        int32 EDX;
    };

    FCPUIDResult CallCPUID(const int32 FunctionId, const int32 SubFunctionId = 0)
    {
        FCPUIDResult Result;
        int32 CPUInfo[4];
        __cpuidex(CPUInfo, FunctionId, SubFunctionId);
        Result.EAX = CPUInfo[0];
        Result.EBX = CPUInfo[1];
        Result.ECX = CPUInfo[2];
        Result.EDX = CPUInfo[3];
        return Result;
    }
}

FCPUInfo FCPUDetection::CPUInfo = {};

void FCPUDetection::Initialize()
{
    DetectVendor();
    DetectBrand();
    DetectCPUTopology();

    CVLOG(LogCPUDetection, Info, "CPU: {}", CPUInfo.BrandString);
    CVLOG(LogCPUDetection, Info, "\t- Vendor: {}", CPUInfo.VendorString);
    CVLOG(LogCPUDetection, Info, "\t- Family: {}", CPUInfo.Family);
    CVLOG(LogCPUDetection, Info, "\t- Model: {}", CPUInfo.Model);
    CVLOG(LogCPUDetection, Info, "\t- Stepping: {}", CPUInfo.Stepping);
    CVLOG(LogCPUDetection, Info, "\t- Physical Cores: {}, Logical Cores: {}", CPUInfo.PhysicalCores, CPUInfo.LogicalCores);
    CVLOG(LogCPUDetection, Info, "\t- Logical Cores: {}", CPUInfo.LogicalCores);
    CVLOG(LogCPUDetection, Info, "\t- Cache Line Size: {} bytes", CPUInfo.CacheLine);
    CVLOG(LogCPUDetection, Info, "\t- L1 Cache: {} KB", CPUInfo.L1CacheSize / 1024);
    CVLOG(LogCPUDetection, Info, "\t- L2 Cache: {} MB", CPUInfo.L2CacheSize / (1024 * 1024));
    CVLOG(LogCPUDetection, Info, "\t- L3 Cache: {} MB", CPUInfo.L3CacheSize / (1024 * 1024));
}

FCPUInfo FCPUDetection::GetCPUInfo() noexcept
{
    return CPUInfo;
}

void FCPUDetection::DetectVendor()
{
    const FCPUIDResult Result = CallCPUID(0, 0);

    char Vendor[13] = {};
    *reinterpret_cast<int32*>(Vendor) = Result.EBX;
    *reinterpret_cast<int32*>(Vendor + 4) = Result.EDX;
    *reinterpret_cast<int32*>(Vendor + 8) = Result.ECX;
    Vendor[12] = '\0';

    CPUInfo.VendorString = FAnsiString(Vendor);
    CPUInfo.IsIntel = CPUInfo.VendorString == "GenuineIntel";
    CPUInfo.IsAMD = CPUInfo.VendorString == "AuthenticAMD";
}

void FCPUDetection::DetectBrand()
{
    char Brand[49] = {};

    const FCPUIDResult Result = CallCPUID(static_cast<int32>(0x80000000), 0);
    if (Result.EAX >= static_cast<int32>(0x80000004))
    {
        const FCPUIDResult Brand0 = CallCPUID(static_cast<int32>(0x80000002), 0);
        const FCPUIDResult Brand1 = CallCPUID(static_cast<int32>(0x80000003), 0);
        const FCPUIDResult Brand2 = CallCPUID(static_cast<int32>(0x80000004), 0);

        std::memcpy(Brand, &Brand0, 16);
        std::memcpy(Brand + 16, &Brand1, 16);
        std::memcpy(Brand + 32, &Brand2, 16);
        Brand[48] = '\0';

        // Trim leading spaces 
        char* Start = Brand;
        while (*Start == ' ') ++Start;

        CPUInfo.BrandString = FAnsiString(Start);
    }
}

void FCPUDetection::DetectCPUTopology()
{
    const FCPUIDResult Result1 = CallCPUID(1, 0);
    CPUInfo.Family = ((Result1.EAX >> 8) & 0xF) + ((Result1.EAX >> 20) & 0xFF);
    CPUInfo.Model = ((Result1.EAX >> 4) & 0xF) | ((Result1.EAX >> 12) & 0xF0);
    CPUInfo.Stepping = Result1.EAX & 0xF;

    SYSTEM_INFO SystemInfo;
    GetSystemInfo(&SystemInfo);
    CPUInfo.LogicalCores = static_cast<int32>(SystemInfo.dwNumberOfProcessors);

    DWORD BufferSize = 0;
    GetLogicalProcessorInformation(nullptr, &BufferSize);
    if (BufferSize > 0)
    {
        const PSYSTEM_LOGICAL_PROCESSOR_INFORMATION Buffer = static_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION>(FMemory::Allocate(BufferSize));
        if (GetLogicalProcessorInformation(Buffer, &BufferSize))
        {
            const DWORD NumEntries = BufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
            int32 PhysicalCores = 0;
            int32 CacheLine = 0;
            int32 L1CacheSize = 0;
            int32 L2CacheSize = 0;
            int32 L3CacheSize = 0;
            for (DWORD Index = 0; Index < NumEntries; ++Index)
            {
                if (Buffer[Index].Relationship == RelationProcessorCore)
                {
                    ++PhysicalCores;
                }
                else if (Buffer[Index].Relationship == RelationCache)
                {
                    if (const CACHE_DESCRIPTOR& Cache = Buffer[Index].Cache; Cache.Level == 1)
                    {
                        if (CacheLine == 0)
                        {
                            CacheLine = static_cast<int32>(Cache.LineSize);
                        }
                        if (Cache.Type == CacheData || Cache.Type == CacheUnified)
                        {
                            L1CacheSize += static_cast<int32>(Cache.Size);
                        }
                        else if (Cache.Type == CacheInstruction)
                        {
                            L1CacheSize += static_cast<int32>(Cache.Size);
                        }
                    }
                    else if (Cache.Level == 2)
                    {
                        L2CacheSize += static_cast<int32>(Cache.Size);
                    }
                    else if (Cache.Level == 3)
                    {
                        if (L3CacheSize == 0)
                        {
                            L3CacheSize = static_cast<int32>(Cache.Size);
                        }
                    }
                }
            }
            CPUInfo.PhysicalCores = PhysicalCores;
            CPUInfo.CacheLine = CacheLine > 0 ? CacheLine : 64;
            CPUInfo.L1CacheSize = L1CacheSize;
            CPUInfo.L2CacheSize = L2CacheSize;
            CPUInfo.L3CacheSize = L3CacheSize;
        }
        FMemory::Free(Buffer);
    }
    else
    {
        CPUInfo.PhysicalCores = CPUInfo.LogicalCores;
        CPUInfo.CacheLine = 64;
        CPUInfo.L1CacheSize = 0;
        CPUInfo.L2CacheSize = 0;
        CPUInfo.L3CacheSize = 0;
    }
    CPUInfo.IsHyperThreaded = CPUInfo.LogicalCores > CPUInfo.PhysicalCores;
}
