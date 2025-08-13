#pragma once

#include "Engine/Core/Containers/Map.hpp"
#include "Engine/Core/Containers/Name.hpp"
#include "Engine/Core/Memory/Memory.hpp"
#include "Engine/Core/Utility/TypeName.hpp"
#include "Engine/Engine/Subsystem.hpp"

#include <functional>
#include <ranges>
#include <type_traits>

#include "Engine/Core/Logging/Logger.hpp"

ENGINE_API DEFINE_LOG_CHANNEL_EXTERN(Subsystem)

template <typename T> requires std::is_base_of_v<ISubsystem, T>
class ENGINE_API FSubsystemCollection
{
public:
    using FLoopFunction = std::function<void(FName, T*)>;

public:
    FSubsystemCollection() = default;

    ~FSubsystemCollection()
    {
        for (T* Subsystem : Subsystems | std::views::values)
        {
            Subsystem->Deinitialize();
            FMemory::DestroyObject<ISubsystem>(Subsystem);
        }
        Subsystems.clear();
    }

    NON_COPY_MOVEABLE(FSubsystemCollection)

public:
    [[nodiscard]] bool8 IsRegistered(FName Name) const
    {
        return Subsystems.contains(Name);
    }

    [[nodiscard]] ISubsystem* GetSubsystem(FName Name) const
    {
        if (!IsRegistered(Name))
        {
            CVLOG(LogSubsystem, Warn, "Unable to get subsystem. Subsystem '{}' is not registered", Name.GetString());
            return nullptr;
        }
        return Subsystems.at(Name);
    }

    bool8 Register(FName Name, T* Subsystem)
    {
        if (IsRegistered(Name))
        {
            CVLOG(LogSubsystem, Warn, "Unable to register subsystem. Subsystem '{}' is already registered", Name.GetString());
            return false;
        }
        Subsystems.emplace(Name, Subsystem);
        Subsystem->Initialize();
        return true;
    }

    bool8 Unregister(FName Name)
    {
        if (!IsRegistered(Name))
        {
            CVLOG(LogSubsystem, Warn, "Unable to unregister subsystem. Subsystem '{}' is not registered", Name.GetString());
            return false;
        }
        T* Subsystem = GetSubsystem(Name);
        Subsystem->Deinitialize();
        Subsystems.erase(Name);
        return true;
    }

    template <typename TSubsystem> requires std::is_base_of_v<T, TSubsystem>
    [[nodiscard]] bool8 IsRegistered() const
    {
        return IsRegistered(GetTypeNameWithoutPrefix<TSubsystem>());
    }

    template <typename TSubsystem> requires std::is_base_of_v<T, TSubsystem>
    [[nodiscard]] TSubsystem* GetSubsystem() const
    {
        return GetSubsystem(GetTypeNameWithoutPrefix<TSubsystem>());
    }

    template <typename TSubsystem, typename... TArguments> requires std::is_base_of_v<T, TSubsystem> && std::is_constructible_v<TSubsystem, TArguments...>
    bool8 Register(TArguments&&... Arguments)
    {
        return Register(GetTypeNameWithoutPrefix<TSubsystem>(), FMemory::NewObject<TSubsystem, TArguments...>(std::forward<TArguments>(Arguments)...));
    }

    template <typename TSubsystem> requires std::is_base_of_v<T, TSubsystem>
    bool8 Unregister()
    {
        return Unregister(GetTypeNameWithoutPrefix<TSubsystem>());
    }

    void ForEach(FLoopFunction&& LoopFunction)
    {
        FLoopFunction MovedFunction = std::move<FLoopFunction>(LoopFunction);
        for (const auto& Entry : Subsystems)
        {
            MovedFunction(Entry.first, Entry.second);
        }
    }

private:
    TUnorderedMap<uint64, T*> Subsystems;
};

#define DEFINE_SUBSYSTEM_FUNCTIONS(SubsystemBaseClass, CollectionVarName) \
    [[nodiscard]] inline bool8 IsSubsystemRegistered(FName Name) const { return CollectionVarName->IsRegistered(Name); } \
    [[nodiscard]] inline ISubsystem* GetSubsystem(FName Name) const { return CollectionVarName->GetSubsystem(Name); } \
    inline bool8 RegisterSubsystem(FName Name, ISubsystem* Subsystem) const { return CollectionVarName->Register(Name, Subsystem); } \
    inline bool8 UnregisterSubsystem(FName Name) const { return CollectionVarName->Unregister(Name); } \
    template <typename T> requires std::is_base_of_v<SubsystemBaseClass, T> \
    [[nodiscard]] inline bool8 IsSubsystemRegistered() const \
    { \
        return CollectionVarName->IsRegistered<T>(); \
    } \
    template <typename T> requires std::is_base_of_v<SubsystemBaseClass, T> \
    [[nodiscard]] inline T* GetSubsystem() \
    { \
        return CollectionVarName->GetSubsystem<T>(); \
    } \
    template <typename T, typename... TArguments> requires std::is_base_of_v<SubsystemBaseClass, T> \
    inline bool8 RegisterSubsystem(TArguments&&... Arguments) \
    { \
        return CollectionVarName->Register<T, TArguments...>(std::forward<TArguments>(Arguments)...); \
    } \
    template <typename T> requires std::is_base_of_v<SubsystemBaseClass, T> \
    inline bool8 UnregisterSubsystem() const \
    { \
        return CollectionVarName->Unregister<T>(); \
    }
