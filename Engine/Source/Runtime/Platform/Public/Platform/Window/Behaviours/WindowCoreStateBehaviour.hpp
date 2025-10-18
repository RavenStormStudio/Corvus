// RavenStorm Copyright @ 2025-2025

#pragma once

#include <Platform/Window/WindowBehaviour.hpp>

class PLATFORM_API FWindowCoreStateBehaviour : public IWindowBehaviour
{
private:
    struct FModalState
    {
        uint32 Width = 0;
        uint32 Height = 0;
        int32 PositionX = 0;
        int32 PositionY = 0;
    };

public:
    FWindowCoreStateBehaviour() = default;
    ~FWindowCoreStateBehaviour() override = default;

    NON_COPY_MOVEABLE(FWindowCoreStateBehaviour)

public:
    [[nodiscard]] bool8 OnWindowEvent(EWindowEvent Event, FWindow* Window, FWindowState& WindowState) override;

private:
    bool8 bIsInModalLoop = false;
    FModalState ModalState = {};
};
