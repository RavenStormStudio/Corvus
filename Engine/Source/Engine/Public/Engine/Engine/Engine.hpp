#pragma once

#include "SubsystemCollection.hpp"

#include "Engine/Core/Utility/Singleton.hpp"

#include <type_traits>

class ENGINE_API FEngine : public TSingleton<FEngine>
{
public:
    FEngine() = default;
    ~FEngine() = default;

    NON_COPY_MOVEABLE(FEngine)

public:
    void Initialize();
    void Shutdown();

public:
    [[nodiscard]] bool8 IsSubsystemRegistered(FName Name) const;
    [[nodiscard]] ISubsystem* GetSubsystem(FName Name) const;
    bool8 RegisterSubsystem(FName Name, ISubsystem* Subsystem) const;
    bool8 UnregisterSubsystem(FName Name) const;

    template <typename T>
    [[nodiscard]] bool8 IsSubsystemRegistered() const
    {
        return SubsystemCollection->IsRegistered<T>();
    }

    template <typename T>
    [[nodiscard]] T* GetSubsystem()
    {
        return SubsystemCollection->GetSubsystem<T>();
    }

    template <typename T, typename... TArguments>
    bool8 RegisterSubsystem(TArguments&&... Arguments)
    {
        return SubsystemCollection->Register<T, TArguments...>(std::forward<TArguments>(Arguments)...);
    }

    template <typename T>
    bool8 UnregisterSubsystem() const
    {
        return SubsystemCollection->Unregister<T>();
    }

private:
    TUniquePtr<FSubsystemCollection> SubsystemCollection;
};

[[nodiscard]] ENGINE_API TSharedPtr<FEngine> GetEngine();
