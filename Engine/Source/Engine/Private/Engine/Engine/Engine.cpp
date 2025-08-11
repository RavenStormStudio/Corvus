#include "Engine/Engine/Engine.hpp"

void FEngine::Initialize()
{
    SubsystemCollection = MakeUnique<FSubsystemCollection>();
}

void FEngine::Shutdown()
{
    SubsystemCollection.reset();
}

bool8 FEngine::IsSubsystemRegistered(const FName Name) const
{
    return SubsystemCollection->IsRegistered(Name);
}

ISubsystem* FEngine::GetSubsystem(const FName Name) const
{
    return SubsystemCollection->GetSubsystem(Name);
}

bool8 FEngine::RegisterSubsystem(const FName Name, ISubsystem* Subsystem) const
{
    return SubsystemCollection->Register(Name, Subsystem);
}

bool8 FEngine::UnregisterSubsystem(const FName Name) const
{
    return SubsystemCollection->Unregister(Name);
}

TSharedPtr<FEngine> GetEngine()
{
    return FEngine::GetInstance();
}
