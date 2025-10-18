// RavenStorm Copyright @ 2025-2025

#pragma once

#include "Platform/Window/WindowBehaviour.hpp"

class PLATFORM_API FWindowCloseBehaviour : public IWindowBehaviour
{
public:
    enum class ECloseAction : uint8
    {
        Allow,
        Prevent,
        PromptUser,
    };

public:
    FWindowCloseBehaviour(ECloseAction InCloseAction = ECloseAction::Allow);
    ~FWindowCloseBehaviour() override = default;

    NON_COPY_MOVEABLE(FWindowCloseBehaviour)

public:
    [[nodiscard]] bool8 OnWindowEvent(EWindowEvent Event, FWindow* Window, FWindowState& WindowState) override;

private:
    bool8 HandleCloseEvent(const FWindow* Window, FWindowState& WindowState) const;

private:
    ECloseAction CloseAction = ECloseAction::Allow;
};
