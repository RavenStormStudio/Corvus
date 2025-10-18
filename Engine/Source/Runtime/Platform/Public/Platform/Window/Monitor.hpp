// RavenStorm Copyright @ 2025-2025

#pragma once

#include "Core/CoreDefinitions.hpp"
#include "Core/Containers/Array.hpp"
#include "Core/Containers/String.hpp"
#include "Core/Utility/WindowsDefinitions.hpp"

struct PLATFORM_API FMonitorRect
{
    int32 Left = 0;
    int32 Right = 0;
    int32 Top = 0;
    int32 Bottom = 0;

public:
    [[nodiscard]] int32 GetWidth() const noexcept;
    [[nodiscard]] int32 GetHeight() const noexcept;

public:
    [[nodiscard]] static FMonitorRect FromNativeRect(Windows::RECT Rect);
};

struct PLATFORM_API FMonitorInfo
{
    FString FriendlyName;
    FString DeviceName;
    FMonitorRect MonitorRect = {};
    FMonitorRect WorkAreaRect = {};
    bool8 bIsPrimary = false;

    FString AdapterName;
    FString DeviceId;
    FString DeviceKey;
    Windows::DWORD StateFlags;
};

struct PLATFORM_API FMonitorDisplayMode
{
    uint32 Width = 0;
    uint32 Height = 0;
    uint32 RefreshRate = 0;
    uint32 BitsPerPixel = 0;

public:
    [[nodiscard]] float64 GetAspectRatio() const;
    [[nodiscard]] uint64 GetPixelCount() const;

public:
    friend bool operator==(const FMonitorDisplayMode& Lhs, const FMonitorDisplayMode& Rhs);
    friend bool operator!=(const FMonitorDisplayMode& Lhs, const FMonitorDisplayMode& Rhs);
};

class PLATFORM_API FMonitor
{
public:
    FMonitor(Windows::HMONITOR InMonitorHandle);
    ~FMonitor();

    NON_COPY_MOVEABLE(FMonitor)

public:
    [[nodiscard]] FMonitorDisplayMode GetActiveDisplayMode() const;

    bool8 SetActiveDisplayMode(FMonitorDisplayMode Mode) const;

public:
    [[nodiscard]] Windows::HMONITOR GetNativeHandle() const { return MonitorHandle; }
    [[nodiscard]] FMonitorInfo GetInfo() const { return MonitorInfo; }
    [[nodiscard]] TArray<FMonitorDisplayMode> GetDisplayModes() const { return DisplayModes; }

private:
    void QueryMonitorInfo();
    void QueryDisplayModes();

private:
    Windows::HMONITOR MonitorHandle = nullptr;

    FMonitorInfo MonitorInfo = {};

    TArray<FMonitorDisplayMode> DisplayModes;
};
