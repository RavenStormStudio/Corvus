#include "Engine/Engine/Engine.hpp"

DECLARE_LOG_CHANNEL(Subsystem, All)

void FEngine::Initialize()
{
    SubsystemCollection = MakeUnique<FSubsystemCollection<IEngineSubsystem>>();
}

void FEngine::Shutdown()
{
    SubsystemCollection.reset();
}

TSharedPtr<FEngine> GetEngine()
{
    return FEngine::GetInstance();
}
