// RavenStorm Copyright @ 2025-2025

#include "Platform/Window/Behaviours/WindowCoreStateBehaviour.hpp"

#include "Platform/Window/Window.hpp"

#include <Windows.h>


bool8 FWindowCoreStateBehaviour::OnWindowEvent(const EWindowEvent Event, FWindow* Window, FWindowState& WindowState)
{
    switch (Event)
    {
    case EWindowEvent::Destroy:
        {
            DestroyWindow(Window->GetHandle());
            Window->WindowHandle = nullptr;
            return false;
        }
    case EWindowEvent::Resize:
        {
            if (!bIsInModalLoop)
            {
                return true;
            }
            const HWND WindowHandle = Window->GetHandle();

            RECT ClientRect = {};
            GetClientRect(WindowHandle, &ClientRect);
            WindowState.ClientWidth = ClientRect.right - ClientRect.left;
            WindowState.ClientHeight = ClientRect.bottom - ClientRect.top;

            RECT WindowRect = {};
            GetWindowRect(WindowHandle, &WindowRect);
            WindowState.WindowWidth = WindowRect.right - WindowRect.left;
            WindowState.WindowHeight = WindowRect.bottom - WindowRect.top;
            return false;
        }
    case EWindowEvent::Move:
        {
            if (!bIsInModalLoop)
            {
                return true;
            }
            const HWND WindowHandle = Window->GetHandle();
            RECT WindowRect = {};
            GetWindowRect(WindowHandle, &WindowRect);

            WindowState.PositionX = WindowRect.left;
            WindowState.PositionY = WindowRect.top;
            return false;
        }
    case EWindowEvent::EnterSizeMove:
        {
            bIsInModalLoop = true;
            ModalState.Width = WindowState.WindowWidth;
            ModalState.Height = WindowState.WindowHeight;
            ModalState.PositionX = WindowState.PositionX;
            ModalState.PositionY = WindowState.PositionY;
            return false;
        }
    case EWindowEvent::ExitSizeMove:
        {
            bIsInModalLoop = false;

            const HWND WindowHandle = Window->GetHandle();
            RECT WindowRect = {};
            GetWindowRect(WindowHandle, &WindowRect);

            FModalState AfterModalState;
            AfterModalState.Width = WindowRect.right - WindowRect.left;
            AfterModalState.Height = WindowRect.bottom - WindowRect.top;
            AfterModalState.PositionX = WindowRect.left;
            AfterModalState.PositionY = WindowRect.top;

            if (AfterModalState.Width != ModalState.Width || AfterModalState.Height != ModalState.Height)
            {
                WindowState.WindowWidth = AfterModalState.Width;
                WindowState.WindowHeight = AfterModalState.Height;

                RECT ClientRect = {};
                GetClientRect(WindowHandle, &ClientRect);
                WindowState.ClientWidth = ClientRect.right - ClientRect.left;
                WindowState.ClientHeight = ClientRect.bottom - ClientRect.top;
            }
            if (AfterModalState.PositionX != ModalState.PositionX || AfterModalState.PositionY != ModalState.PositionY)
            {
                WindowState.PositionX = AfterModalState.PositionX;
                WindowState.PositionY = AfterModalState.PositionY;
            }
            return false;
        }
    default:
        break;
    }
    return false;
}
