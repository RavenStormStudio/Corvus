// RavenStorm Copyright @ 2025-2025

#include "Platform/Window/Monitor.hpp"

#include <Windows.h>

#include "Core/Containers/Ranges.hpp"
#include "Core/Logging/LogManager.hpp"

int32 FMonitorRect::GetWidth() const noexcept
{
    return Right - Left;
}

int32 FMonitorRect::GetHeight() const noexcept
{
    return Bottom - Top;
}

FMonitorRect FMonitorRect::FromNativeRect(const Windows::RECT Rect)
{
    FMonitorRect Result;
    Result.Left = Rect.left;
    Result.Top = Rect.top;
    Result.Right = Rect.right;
    Result.Bottom = Rect.bottom;
    return Result;
}

float64 FMonitorDisplayMode::GetAspectRatio() const
{
    return static_cast<float64>(Width) / static_cast<float64>(Height);
}

uint64 FMonitorDisplayMode::GetPixelCount() const
{
    return static_cast<uint64>(Width) * static_cast<uint64>(Height);
}

FMonitor::FMonitor(const Windows::HMONITOR InMonitorHandle)
    : MonitorHandle(InMonitorHandle)
{
    QueryMonitorInfo();
    QueryDisplayModes();
}

FMonitor::~FMonitor() = default;

FMonitorDisplayMode FMonitor::GetActiveDisplayMode() const
{
    DEVMODE DevMode = {};
    DevMode.dmSize = sizeof(DEVMODE);

    FMonitorDisplayMode DisplayMode = {};
    while (EnumDisplaySettings(MonitorInfo.DeviceName.c_str(), ENUM_CURRENT_SETTINGS, &DevMode))
    {
        DisplayMode.Width = DevMode.dmPelsWidth;
        DisplayMode.Height = DevMode.dmPelsHeight;
        DisplayMode.RefreshRate = DevMode.dmDisplayFrequency;
        DisplayMode.BitsPerPixel = DevMode.dmBitsPerPel;
        break;
    }
    return DisplayMode;
}

bool8 FMonitor::SetActiveDisplayMode(FMonitorDisplayMode Mode) const
{
    DEVMODE DevMode = {};
    DevMode.dmSize = sizeof(DEVMODE);
    DevMode.dmPelsWidth = Mode.Width;
    DevMode.dmPelsHeight = Mode.Height;
    DevMode.dmDisplayFrequency = Mode.RefreshRate;
    DevMode.dmBitsPerPel = Mode.BitsPerPixel;
    DevMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY | DM_BITSPERPEL;

    LONG Result = ChangeDisplaySettingsEx(MonitorInfo.DeviceName.c_str(), &DevMode, nullptr, CDS_UPDATEREGISTRY, nullptr);
    if (Result != DISP_CHANGE_SUCCESSFUL)
    {
        CVLOG(LogTemp, Error, TEXT("Failed to set display mode: {}x{} @{}Hz (Error: {})"), Mode.Width, Mode.Height, Mode.RefreshRate, Result);
        return false;
    }
    return true;
}

void FMonitor::QueryMonitorInfo()
{
    MONITORINFOEX NativeMonitorInfo = {};
    NativeMonitorInfo.cbSize = sizeof(MONITORINFOEX);

    if (GetMonitorInfo(MonitorHandle, &NativeMonitorInfo))
    {
        MonitorInfo.DeviceName = NativeMonitorInfo.szDevice;
        MonitorInfo.MonitorRect = FMonitorRect::FromNativeRect(NativeMonitorInfo.rcMonitor);
        MonitorInfo.WorkAreaRect = FMonitorRect::FromNativeRect(NativeMonitorInfo.rcWork);
        MonitorInfo.bIsPrimary = (NativeMonitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0;

        DISPLAY_DEVICE DisplayDevice = {};
        DisplayDevice.cb = sizeof(DISPLAY_DEVICE);
        if (EnumDisplayDevices(NativeMonitorInfo.szDevice, 0, &DisplayDevice, 0))
        {
            MonitorInfo.FriendlyName = DisplayDevice.DeviceString;
            MonitorInfo.AdapterName = DisplayDevice.DeviceName;
            MonitorInfo.DeviceId = DisplayDevice.DeviceID;
            MonitorInfo.DeviceKey = DisplayDevice.DeviceKey;
            MonitorInfo.StateFlags = DisplayDevice.StateFlags;
        }
    }
}

void FMonitor::QueryDisplayModes()
{
    DEVMODE DevMode = {};
    DevMode.dmSize = sizeof(DEVMODE);

    DWORD ModeIndex = 0;
    while (EnumDisplaySettings(MonitorInfo.DeviceName.c_str(), ModeIndex++, &DevMode))
    {
        FMonitorDisplayMode DisplayMode = {};
        DisplayMode.Width = DevMode.dmPelsWidth;
        DisplayMode.Height = DevMode.dmPelsHeight;
        DisplayMode.RefreshRate = DevMode.dmDisplayFrequency;
        DisplayMode.BitsPerPixel = DevMode.dmBitsPerPel;
        if (!Ranges::Contains(DisplayModes, DisplayMode))
        {
            DisplayModes.PushBack(DisplayMode);
        }
    }
}

bool operator==(const FMonitorDisplayMode& Lhs, const FMonitorDisplayMode& Rhs)
{
    return Lhs.Width == Rhs.Width && Lhs.Height == Rhs.Height && Lhs.RefreshRate == Rhs.RefreshRate && Lhs.BitsPerPixel == Rhs.BitsPerPixel;
}

bool operator!=(const FMonitorDisplayMode& Lhs, const FMonitorDisplayMode& Rhs)
{
    return !(Lhs == Rhs);
}
