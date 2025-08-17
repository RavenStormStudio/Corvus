#pragma once

#include "Engine/Core/CoreDefinitions.hpp"
#include "Engine/Core/CoreTypes.hpp"
#include "Engine/Core/Containers/Map.hpp"

#include "Function.hpp"

#include <ranges>
#include <type_traits>

struct FDelegateHandle
{
public:
    constexpr FDelegateHandle() noexcept = default;

    constexpr explicit FDelegateHandle(const uint64 InId) noexcept
        : Id(InId)
    {
    }

    constexpr ~FDelegateHandle() noexcept = default;

    DEFAULT_COPY_MOVEABLE_PREFIX(FDelegateHandle, constexpr)

public:
    [[nodiscard]] constexpr bool8 IsValid() const noexcept { return Id != 0; }
    constexpr void Reset() noexcept { Id = 0; }
    [[nodiscard]] uint64 GetId() const { return Id; }

    constexpr auto operator<=>(const FDelegateHandle&) const noexcept = default;

public:
    [[nodiscard]] static FDelegateHandle NewHandle() noexcept
    {
        static uint64 IdCounter = 1;
        return FDelegateHandle(IdCounter++);
    }

private:
    uint64 Id = 0;
};

template <>
struct std::hash<FDelegateHandle>
{
    std::size_t operator()(const FDelegateHandle& Handle) const noexcept
    {
        return std::hash<uint64>{}(Handle.GetId());
    }
};

template <typename TCallable, typename TReturn, typename... TArguments>
concept CCallableWith = std::is_invocable_v<TCallable, TArguments...> && (std::is_void_v<TReturn> || std::is_convertible_v<std::invoke_result_t<TCallable, TArguments...>, TReturn>);

template <typename TSignature>
class TDelegate;

template <typename TReturn, typename... TArguments>
class TDelegate<TReturn(TArguments...)>
{
public:
    using FSignature = TReturn(TArguments...);
    using FFunction = TFunction<FSignature>;

public:
    TDelegate() = default;
    ~TDelegate() = default;

    DEFAULT_COPY_MOVEABLE(TDelegate)

public:
    template <typename TCallable> requires CCallableWith<TCallable, TReturn, TArguments...>
    void Bind(TCallable&& Callable)
    {
        Function = std::forward<TCallable>(Callable);
    }

    template <typename TObject>
    void BindMember(TObject* Object, TReturn (TObject::*FunctionPointer)(TArguments...))
    {
        Function = [Object, FunctionPointer](TArguments... Arguments) -> TReturn
        {
            return std::invoke(FunctionPointer, Object, Arguments...);
        };
    }

    template <typename TObject>
    void BindMember(const TObject* Object, TReturn (TObject::*FunctionPointer)(TArguments...) const)
    {
        Function = [Object, FunctionPointer](TArguments... Arguments) -> TReturn
        {
            return std::invoke(FunctionPointer, Object, Arguments...);
        };
    }

    TReturn Execute(TArguments... Arguments) const requires !std::is_same_v<void, TReturn>
    {
        if (Function)
        {
            return Function(Arguments...);
        }
        return TReturn{};
    }

    void Execute(TArguments... Arguments) const requires std::is_same_v<void, TReturn>
    {
        if (Function)
        {
            Function(Arguments...);
        }
    }

    [[nodiscard]] bool8 IsBound() const noexcept
    {
        return Function != nullptr;
    }

    void Unbind() noexcept
    {
        Function = nullptr;
    }

private:
    FFunction Function;
};

template <typename TSignature>
class TMulticastDelegate;

template <typename TReturn, typename... TArguments> requires std::is_same_v<TReturn, void>
class TMulticastDelegate<TReturn(TArguments...)>
{
public:
    using FSignature = TReturn(TArguments...);
    using FFunction = TFunction<FSignature>;

public:
    TMulticastDelegate() = default;
    ~TMulticastDelegate() = default;

    NON_COPYABLE(TMulticastDelegate,)
    DEFAULT_MOVEABLE(TMulticastDelegate,)

public:
    template <typename TCallable> requires CCallableWith<std::decay_t<TCallable>, TReturn, TArguments...>
    [[nodiscard]] FDelegateHandle AddBinding(TCallable&& Callable)
    {
        FDelegateHandle Handle = FDelegateHandle::NewHandle();
        Functions.emplace(Handle, std::forward<TCallable>(Callable));
        return Handle;
    }

    template <typename TClass>
    [[nodiscard]] FDelegateHandle AddMemberBinding(TClass* Class, TReturn (TClass::*Method)(TArguments...))
    {
        return AddBinding([Class, Method](TArguments... Arguments)
        {
            std::invoke(Method, Class, Arguments...);
        });
    }

    template <typename TClass>
    [[nodiscard]] FDelegateHandle AddMemberBinding(const TClass* Class, TReturn (TClass::*Method)(TArguments...) const)
    {
        return AddBinding([Class, Method](TArguments... Arguments)
        {
            std::invoke(Method, Class, Arguments...);
        });
    }

    bool8 RemoveBinding(FDelegateHandle& Handle)
    {
        if (!Functions.contains(Handle))
        {
            return false;
        }
        Functions.erase(Handle);
        Handle.Reset();
        return true;
    }

    void Clear()
    {
        Functions.clear();
    }

    void Broadcast(TArguments... Arguments) const
    {
        if (Functions.empty())
        {
            return;
        }
        for (const FFunction& Function : Functions | std::views::values)
        {
            if (Function)
            {
                Function(Arguments...);
            }
        }
    }

    [[nodiscard]] bool8 IsBound() const noexcept
    {
        return !Functions.empty();
    }

    [[nodiscard]] size64 GetBindingsCount() const noexcept
    {
        return Functions.size();
    }

    [[nodiscard]] bool8 IsHandleBound(FDelegateHandle Handle)
    {
        return Functions.contains(Handle);
    }

public:
    TUnorderedMap<FDelegateHandle, FFunction> Functions;
};

#define DECLARE_DELEGATE(Name, ...) using Name##Delegate = TDelegate<void(__VA_ARGS__)>
#define DECLARE_DELEGATE_RETURN(Name, ReturnType, ...) using Name##Delegate = TDelegate<ReturnType(__VA_ARGS__)>
#define DECLARE_MULTICAST_DELEGATE(Name, ...) using Name##Delegate = TMulticastDelegate<void(__VA_ARGS__)>
