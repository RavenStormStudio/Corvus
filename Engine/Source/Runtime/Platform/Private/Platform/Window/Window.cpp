// RavenStorm Copyright @ 2025-2025

#include "Platform/Window/Window.hpp"

#include "Core/Assertion/Assertion.hpp"

#include <Windows.h>

#include "Core/Containers/Ranges.hpp"

#include "Platform/Window/Behaviours/WindowCloseBehaviour.hpp"
#include "Platform/Window/Behaviours/WindowCoreStateBehaviour.hpp"

LRESULT MainWindowProcess(const HWND ProcessWindowHandle, const uint32 Message, const WPARAM WParam, const LPARAM LParam)
{
    FWindow* Window;
    if (Message == WM_NCCREATE)
    {
        const CREATESTRUCT* CreateStruct = reinterpret_cast<CREATESTRUCT*>(LParam);
        Window = static_cast<FWindow*>(CreateStruct->lpCreateParams);
        SetWindowLongPtr(ProcessWindowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(Window));
    }
    else
    {
        Window = reinterpret_cast<FWindow*>(GetWindowLongPtr(ProcessWindowHandle, GWLP_USERDATA));
    }

    if (Window)
    {
        return Window->WindowProcess(ProcessWindowHandle, Message, WParam, LParam);
    }
    return DefWindowProc(ProcessWindowHandle, Message, WParam, LParam);
}

FWindow::FWindow(const FWindowSettings& InWindowSettings)
{
    WNDCLASSEX WindowClass = {};
    WindowClass.cbSize = sizeof(WNDCLASSEX);
    WindowClass.style = CS_HREDRAW | CS_VREDRAW;
    WindowClass.lpfnWndProc = MainWindowProcess;
    WindowClass.hInstance = GetModuleHandle(nullptr);
    WindowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    WindowClass.lpszClassName = TEXT("CorvusWindowClass");
    RegisterClassEx(&WindowClass);

    State.WindowWidth = InWindowSettings.Width;
    State.WindowHeight = InWindowSettings.Height;
    State.ClientWidth = InWindowSettings.Width;
    State.ClientHeight = InWindowSettings.Height;
    State.PositionX = InWindowSettings.StartPositionX;
    State.PositionY = InWindowSettings.StartPositionY;
    State.Title = InWindowSettings.Title;
    State.WindowMode = InWindowSettings.WindowMode;
    State.ParentWindow = InWindowSettings.ParentWindow;

    AddBehaviour<FWindowCoreStateBehaviour>();

    FWindowStyle WindowStyle = GetWindowStyle(InWindowSettings.WindowMode);
    if (WindowStyle.Style & WS_BORDER)
    {
        RECT WindowRect = {0, 0, static_cast<LONG>(State.ClientWidth), static_cast<LONG>(State.ClientHeight)};
        AdjustWindowRectEx(&WindowRect, WindowStyle.Style, false, WindowStyle.ExStyle);
        State.WindowWidth = WindowRect.right - WindowRect.left;
        State.WindowHeight = WindowRect.bottom - WindowRect.top;
    }

    HWND ParentWindowHandle = nullptr;
    if (InWindowSettings.ParentWindow)
    {
        ParentWindowHandle = InWindowSettings.ParentWindow->GetHandle();
        WindowStyle.Style |= WS_CHILD;
    }
    WindowHandle = CreateWindowEx(WindowStyle.ExStyle,
                                  WindowClass.lpszClassName,
                                  State.Title.c_str(),
                                  WindowStyle.Style,
                                  State.PositionX,
                                  State.PositionY,
                                  static_cast<int32>(State.WindowWidth),
                                  static_cast<int32>(State.WindowHeight),
                                  ParentWindowHandle,
                                  nullptr,
                                  GetModuleHandle(nullptr),
                                  this);
    if (WindowHandle == nullptr)
    {
        const HRESULT Result = static_cast<HRESULT>(GetLastError());
        ASSERT_RESULT(Result);
    }

    if (InWindowSettings.bShowOnCreation)
    {
        Show();
    }
}

FWindow::~FWindow()
{
    if (WindowHandle)
    {
        Close();
        DestroyWindow(WindowHandle);
        WindowHandle = nullptr;
    }
}

bool8 FWindow::IsValid() const noexcept
{
    return WindowHandle != nullptr;
}

void FWindow::Show() const
{
    ShowWindowAsync(WindowHandle, SW_SHOW);
}

void FWindow::Hide() const
{
    ShowWindowAsync(WindowHandle, SW_HIDE);
}

void FWindow::Close() const
{
    CloseWindow(WindowHandle);
}

void FWindow::Maximize() const
{
    ShowWindowAsync(WindowHandle, SW_MAXIMIZE);
}

void FWindow::Minimize() const
{
    ShowWindowAsync(WindowHandle, SW_MINIMIZE);
}

void FWindow::Restore() const
{
    ShowWindowAsync(WindowHandle, SW_RESTORE);
}

void FWindow::Focus() const
{
    SetForegroundWindow(WindowHandle);
    SetFocus(WindowHandle);
}

void FWindow::SetWindowSize(const uint32 NewWidth, const uint32 NewHeight) const
{
    SetWindowPos(WindowHandle, nullptr, 0, 0, static_cast<int32>(NewWidth), static_cast<int32>(NewHeight), SWP_NOMOVE | SWP_NOZORDER);
}

void FWindow::SetClientSize(const uint32 NewWidth, const uint32 NewHeight) const
{
    RECT WindowRect = {0, 0, static_cast<LONG>(NewWidth), static_cast<LONG>(NewHeight)};
    if (AdjustWindowRect(&WindowRect, GetWindowLong(WindowHandle, GWL_STYLE), false))
    {
        const int32 AdjustedWidth = WindowRect.right - WindowRect.left;
        const int32 AdjustedHeight = WindowRect.bottom - WindowRect.top;
        SetWindowPos(WindowHandle, nullptr, 0, 0, AdjustedWidth, AdjustedHeight, SWP_NOMOVE);
    }
}

void FWindow::SetPosition(const int32 NewPositionX, const int32 NewPositionY) const
{
    SetWindowPos(WindowHandle, nullptr, NewPositionX, NewPositionY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void FWindow::SetTitle(const FString& NewTitle)
{
    State.Title = NewTitle;
    SetWindowText(WindowHandle, State.Title.c_str());
}

void FWindow::AppendTitle(const FString& NewTitle)
{
    State.Title += NewTitle;
    SetWindowText(WindowHandle, State.Title.c_str());
}

void FWindow::SetParentWindow(const TSharedPtr<FWindow>& NewParentWindow)
{
    State.ParentWindow = NewParentWindow;
    if (State.ParentWindow != nullptr)
    {
        SetParent(WindowHandle, State.ParentWindow->GetHandle());
    }
    else
    {
        SetParent(WindowHandle, nullptr);
    }
}

void FWindow::AddBehaviour(TSharedPtr<IWindowBehaviour>&& Behaviour)
{
    Behaviours.EmplaceBack(std::move(Behaviour));
}

Windows::LRESULT FWindow::WindowProcess(const Windows::HWND ProcessWindowHandle, const uint32 Message, const Windows::WPARAM WParam, const Windows::LPARAM LParam)
{
    switch (Message)
    {
    case WM_CLOSE:
        ExecuteBehaviours(EWindowEvent::Close);
        return 0;
    case WM_DESTROY:
        ExecuteBehaviours(EWindowEvent::Destroy);
        return 0;
    case WM_SIZE:
        ExecuteBehaviours(EWindowEvent::Resize);
        return 0;
    case WM_MOVE:
        ExecuteBehaviours(EWindowEvent::Move);
        return 0;
    case WM_ENTERSIZEMOVE:
        ExecuteBehaviours(EWindowEvent::EnterSizeMove);
        return 0;
    case WM_EXITSIZEMOVE:
        ExecuteBehaviours(EWindowEvent::ExitSizeMove);
        return 0;
    default:
        break;
    }
    return DefWindowProc(ProcessWindowHandle, Message, WParam, LParam);
}

bool8 FWindow::ExecuteBehaviours(const EWindowEvent Event)
{
    return Ranges::AnyOf(Behaviours, [&](const TSharedPtr<IWindowBehaviour>& Behaviour)
    {
        return Behaviour->OnWindowEvent(Event, this, State);
    });
}

FWindow::FWindowStyle FWindow::GetWindowStyle(const EWindowMode& WindowMode)
{
    switch (WindowMode)
    {
    case EWindowMode::Windowed:
        return {.Style = WS_OVERLAPPEDWINDOW, .ExStyle = WS_EX_OVERLAPPEDWINDOW};
    case EWindowMode::BorderlessFullscreen:
        return {.Style = 0, .ExStyle = WS_EX_OVERLAPPEDWINDOW};
    case EWindowMode::Fullscreen:
        return {.Style = 0, .ExStyle = WS_EX_OVERLAPPEDWINDOW};
    }
    return {.Style = 0, .ExStyle = 0};
}
