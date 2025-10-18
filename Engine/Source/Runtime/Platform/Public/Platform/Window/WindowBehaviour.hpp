// RavenStorm Copyright @ 2025-2025

#pragma once

#include "Core/CoreDefinitions.hpp"

class FWindow;
struct FWindowState;

enum class PLATFORM_API EWindowEvent : uint8
{
    None = 0,
    Close,
    Destroy,
    Resize,
    Move,
    EnterSizeMove,
    ExitSizeMove,
};

class PLATFORM_API IWindowBehaviour
{
public:
    IWindowBehaviour() = default;
    virtual ~IWindowBehaviour() = default;

    NON_COPY_MOVEABLE(IWindowBehaviour)

public:
    [[nodiscard]] virtual bool8 OnWindowEvent(EWindowEvent Event, FWindow* Window, FWindowState& WindowState) = 0;
};
