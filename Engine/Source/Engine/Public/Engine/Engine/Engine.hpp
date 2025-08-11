#pragma once

#include "SubsystemCollection.hpp"

#include "Engine/Core/Utility/Singleton.hpp"

#include <type_traits>

using IEngineSubsystem = ISubsystem;

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
    DEFINE_SUBSYSTEM_FUNCTIONS(IEngineSubsystem, SubsystemCollection)

private:
    TUniquePtr<FSubsystemCollection<IEngineSubsystem>> SubsystemCollection;
};

[[nodiscard]] ENGINE_API TSharedPtr<FEngine> GetEngine();
