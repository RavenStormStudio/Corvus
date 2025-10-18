// RavenStorm Copyright @ 2025-2025

#include "Platform/Window/Behaviours/WindowCloseBehaviour.hpp"

#include "Platform/Window/Window.hpp"

#include <Windows.h>

bool8 PromptUserForClose(const FWindow* Window)
{
    const int32 Result = MessageBox(Window->GetHandle(), TEXT("Are you sure you want to close this window?"), TEXT("Corvus"), MB_YESNO | MB_ICONQUESTION);
    return Result == IDYES;
}

FWindowCloseBehaviour::FWindowCloseBehaviour(const ECloseAction InCloseAction)
    : CloseAction(InCloseAction)
{
}

bool8 FWindowCloseBehaviour::OnWindowEvent(const EWindowEvent Event, FWindow* Window, FWindowState& WindowState)
{
    switch (Event)
    {
    case EWindowEvent::Close:
        return HandleCloseEvent(Window, WindowState);
    default:
        break;
    }
    return false;
}

bool8 FWindowCloseBehaviour::HandleCloseEvent(const FWindow* Window, FWindowState& WindowState) const
{
    switch (CloseAction)
    {
    case ECloseAction::Allow:
        WindowState.bIsClosed = true;
        return false;
    case ECloseAction::Prevent:
        WindowState.bIsClosed = false;
        return true;
    case ECloseAction::PromptUser:
        if (PromptUserForClose(Window))
        {
            WindowState.bIsClosed = true;
            return false;
        }
        WindowState.bIsClosed = false;
        return true;
    }
    return false;
}
