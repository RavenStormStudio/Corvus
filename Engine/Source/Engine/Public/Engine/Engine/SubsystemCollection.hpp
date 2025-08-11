#pragma once

#include "Engine/Core/Containers/Map.hpp"
#include "Engine/Core/Containers/Name.hpp"
#include "Engine/Core/Memory/Memory.hpp"
#include "Engine/Core/Utility/TypeName.hpp"

class ISubsystem;

class ENGINE_API FSubsystemCollection
{
public:
    FSubsystemCollection();
    ~FSubsystemCollection();

    NON_COPY_MOVEABLE(FSubsystemCollection)

public:
    [[nodiscard]] bool8 IsRegistered(FName Name) const;
    [[nodiscard]] ISubsystem* GetSubsystem(FName Name) const;
    bool8 Register(FName Name, ISubsystem* Subsystem);
    bool8 Unregister(FName Name);

    template <typename T> requires std::is_base_of_v<ISubsystem, T>
    [[nodiscard]] bool8 IsRegistered() const
    {
        return IsRegistered(GetTypeNameWithoutPrefix<T>());
    }

    template <typename T> requires std::is_base_of_v<ISubsystem, T>
    [[nodiscard]] T* GetSubsystem() const
    {
        return GetSubsystem(GetTypeNameWithoutPrefix<T>());
    }

    template <typename T, typename... TArguments> requires std::is_base_of_v<ISubsystem, T> && std::is_constructible_v<T, TArguments...>
    bool8 Register(TArguments&&... Arguments)
    {
        return Register(GetTypeNameWithoutPrefix<T>(), FMemory::NewObject<T, TArguments...>(std::forward<TArguments>(Arguments)...));
    }

    template <typename T> requires std::is_base_of_v<ISubsystem, T>
    bool8 Unregister()
    {
        return Unregister(GetTypeNameWithoutPrefix<T>());
    }

private:
    TUnorderedMap<uint64, ISubsystem*> Subsystems;
};
