// RavenStorm Copyright @ 2025-2025

#pragma once

#include "Core/CoreDefinitions.hpp"
#include "Core/Containers/Array.hpp"
#include "Core/Containers/String.hpp"
#include "Core/Memory/SmartPointers.hpp"
#include "Core/Utility/WindowsDefinitions.hpp"

#include "Platform/Window/WindowBehaviour.hpp"

class FWindow;

enum class PLATFORM_API EWindowMode : uint8
{
    Windowed,
    BorderlessFullscreen,
    Fullscreen,
};

struct PLATFORM_API FWindowSettings
{
    uint32 Width = 1280;
    uint32 Height = 720;
    int32 StartPositionX = 100;
    int32 StartPositionY = 100;
    FString Title = TEXT("Corvus");
    EWindowMode WindowMode = EWindowMode::Windowed;
    TSharedPtr<FWindow> ParentWindow = nullptr;
    bool8 bShowOnCreation = true;
};

struct PLATFORM_API FWindowState
{
    uint32 WindowWidth = 0;
    uint32 WindowHeight = 0;
    uint32 ClientWidth = 0;
    uint32 ClientHeight = 0;
    int32 PositionX = 0;
    int32 PositionY = 0;
    FString Title = TEXT("Corvus");
    EWindowMode WindowMode = EWindowMode::Windowed;
    bool8 bIsClosed = false;
    TSharedPtr<FWindow> ParentWindow = nullptr;
};

class PLATFORM_API FWindow
{
private:
    struct FWindowStyle
    {
        Windows::DWORD Style = 0;
        Windows::DWORD ExStyle = 0;
    };

    struct FModalState
    {
        uint32 Width = 0;
        uint32 Height = 0;
        int32 PositionX = 0;
        int32 PositionY = 0;
    };

public:
    FWindow(const FWindowSettings& InWindowSettings);
    ~FWindow();

    NON_COPY_MOVEABLE(FWindow)

public:
    [[nodiscard]] bool8 IsValid() const noexcept;

    void Show() const;
    void Hide() const;
    void Close() const;
    void Maximize() const;
    void Minimize() const;
    void Restore() const;
    void Focus() const;

    void SetWindowSize(uint32 NewWidth, uint32 NewHeight) const;
    void SetClientSize(uint32 NewWidth, uint32 NewHeight) const;
    void SetPosition(int32 NewPositionX, int32 NewPositionY) const;
    void SetTitle(const FString& NewTitle);
    void AppendTitle(const FString& NewTitle);
    void SetParentWindow(const TSharedPtr<FWindow>& NewParentWindow);

    void AddBehaviour(TSharedPtr<IWindowBehaviour>&& Behaviour);

public:
    template <typename T, typename... TArguments> requires std::is_base_of_v<IWindowBehaviour, T> && std::is_constructible_v<T, TArguments...>
    void AddBehaviour(TArguments&&... Arguments)
    {
        AddBehaviour(std::move(MakeShared<T>(std::forward<TArguments>(Arguments)...)));
    }

public:
    [[nodiscard]] Windows::HWND GetHandle() const { return WindowHandle; }

    [[nodiscard]] FWindowState GetState() const { return State; }

private:
    Windows::LRESULT WindowProcess(Windows::HWND ProcessWindowHandle, uint32 Message, Windows::WPARAM WParam, Windows::LPARAM LParam);

    bool8 ExecuteBehaviours(EWindowEvent Event);

private:
    static FWindowStyle GetWindowStyle(const EWindowMode& WindowMode);

private:
    FWindowState State = {};

    Windows::HWND WindowHandle = nullptr;

    bool8 bIsInModalLoop = false;
    FModalState ModalState = {};

    TArray<TSharedPtr<IWindowBehaviour>> Behaviours;

    friend Windows::LRESULT MainWindowProcess(Windows::HWND, uint32, Windows::WPARAM, Windows::LPARAM);
    friend class PLATFORM_API FWindowCoreStateBehaviour;
};
