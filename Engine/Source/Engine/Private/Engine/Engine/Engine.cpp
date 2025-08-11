#include "Engine/Engine/Engine.hpp"

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
