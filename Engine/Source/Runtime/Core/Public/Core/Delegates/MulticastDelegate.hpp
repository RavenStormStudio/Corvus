// RavenStorm Copyright @ 2025-2025

#pragma once

#include <functional>
#include <ranges>
#include <type_traits>

#include "Core/CoreDefinitions.hpp"
#include "Core/Containers/Map.hpp"
#include "Core/Logging/LogManager.hpp"
#include "Core/Memory/SmartPointers.hpp"

#define CV_INVALID_DELEGATE_HANDLE FORWARD(0)

struct FDelegateHandle
{
public:
    constexpr FDelegateHandle() = default;

    constexpr FDelegateHandle(const uint64 InId)
        : Id(InId)
    {
    }

    constexpr ~FDelegateHandle() = default;

    DEFAULT_COPY_MOVEABLE_PREFIX(FDelegateHandle, constexpr)

public:
    [[nodiscard]] constexpr bool8 IsValid() const
    {
        return Id != CV_INVALID_DELEGATE_HANDLE;
    }

public:
    [[nodiscard]] constexpr uint64 GetId() const { return Id; }

public:
    constexpr operator uint64() const
    {
        return GetId();
    }

    constexpr operator bool8() const
    {
        return IsValid();
    }

private:
    uint64 Id = CV_INVALID_DELEGATE_HANDLE;
};

template <typename... TParameters>
class TMulticastDelegate
{
public:
    using TFuncType = std::function<void(TParameters...)>;

public:
    TMulticastDelegate() = default;
    ~TMulticastDelegate() = default;

    NON_COPY_MOVEABLE(TMulticastDelegate)

public:
    [[nodiscard]] bool8 IsBound() const noexcept
    {
        return !Functions.IsEmpty();
    }

    [[nodiscard]] FDelegateHandle Bind(TFuncType&& Function)
    {
        const FDelegateHandle Handle(GetNextHandleId());
        Functions.Emplace(Handle.GetId(), std::move(Function));
        return Handle;
    }

    template <typename TOwner>
    [[nodiscard]] FDelegateHandle BindRaw(TOwner* Owner, void (TOwner::*Function)(TParameters...))
    {
        return Bind([Owner, Function](TParameters... Args)
        {
            (Owner->*Function)(std::forward<TParameters>(Args)...);
        });
    }

    template <typename TOwner>
    [[nodiscard]] FDelegateHandle BindRaw(const TOwner* Owner, void (TOwner::*Function)(TParameters...) const)
    {
        return Bind([Owner, Function](TParameters... Args)
        {
            (Owner->*Function)(std::forward<TParameters>(Args)...);
        });
    }

    template <typename TOwner>
    [[nodiscard]] FDelegateHandle BindShared(TSharedPtr<TOwner> Owner, void (TOwner::*MemberFunction)(TParameters...))
    {
        return Bind([Owner, MemberFunction](TParameters... Parameters)
        {
            (Owner.get()->*MemberFunction)(std::forward<TParameters>(Parameters)...);
        });
    }

    template <typename TOwner>
    [[nodiscard]] FDelegateHandle BindShared(const TSharedPtr<TOwner> Owner, void (TOwner::*MemberFunction)(TParameters...) const)
    {
        return Bind([Owner, MemberFunction](TParameters... Parameters)
        {
            (Owner.get()->*MemberFunction)(std::forward<TParameters>(Parameters)...);
        });
    }

    template <typename TOwner>
    [[nodiscard]] FDelegateHandle BindWeak(TWeakPtr<TOwner> WeakOwner, void (TOwner::*MemberFunction)(TParameters...))
    {
        return Bind([WeakOwner, MemberFunction](TParameters... Parameters)
        {
            if (auto Owner = WeakOwner.lock())
            {
                (Owner.get()->*MemberFunction)(std::forward<TParameters>(Parameters)...);
            }
        });
    }

    template <typename TOwner>
    [[nodiscard]] FDelegateHandle BindWeak(const TWeakPtr<TOwner> WeakOwner, void (TOwner::*MemberFunction)(TParameters...) const)
    {
        return Bind([WeakOwner, MemberFunction](TParameters... Parameters)
        {
            if (auto Owner = WeakOwner.lock())
            {
                (Owner.get()->*MemberFunction)(std::forward<TParameters>(Parameters)...);
            }
        });
    }

    bool8 Unbind(const FDelegateHandle& Handle)
    {
        if (!Functions.Contains(Handle.GetId()))
        {
            CVLOG(LogTemp, Warning, "There is no handle with id '{}' registered on this delegate", Handle.GetId());
            return false;
        }
        Functions.Remove(Handle.GetId());
        return true;
    }

    void Clear()
    {
        Functions.Clear();
    }

    template <typename... TOtherParameters>
        requires (sizeof...(TOtherParameters) == sizeof...(TParameters) && (std::convertible_to<TOtherParameters&&, TParameters> && ...))
    void Broadcast(TOtherParameters&&... Parameters)
    {
        for (TFuncType& Function : Functions | std::views::values)
        {
            Function.operator()(std::forward<TOtherParameters>(Parameters)...);
        }
    }

private:
    [[nodiscard]] uint64 GetNextHandleId()
    {
        ++NextHandleId;
        if (NextHandleId == CV_INVALID_DELEGATE_HANDLE)
        {
            ++NextHandleId;
        }
        return NextHandleId;
    }

private:
    TMap<uint64, TFuncType> Functions;
    uint64 NextHandleId = 0;
};
