#include "Engine/Engine/SubsystemCollection.hpp"

#include <ranges>

#include "Engine/Engine/Subsystem.hpp"

FSubsystemCollection::FSubsystemCollection() = default;

FSubsystemCollection::~FSubsystemCollection()
{
    for (ISubsystem* Subsystem : Subsystems | std::views::values)
    {
        Subsystem->Deinitialize();
        FMemory::DestroyObject<ISubsystem>(Subsystem);
    }
    Subsystems.clear();
}

bool8 FSubsystemCollection::IsRegistered(const FName Name) const
{
    return Subsystems.contains(Name);
}

ISubsystem* FSubsystemCollection::GetSubsystem(const FName Name) const
{
    if (!IsRegistered(Name))
    {
        return nullptr;
    }
    return Subsystems.at(Name);
}

bool8 FSubsystemCollection::Register(FName Name, ISubsystem* Subsystem)
{
    if (IsRegistered(Name))
    {
        return false;
    }
    Subsystems.emplace(Name, Subsystem);
    Subsystem->Initialize();
    return true;
}

bool8 FSubsystemCollection::Unregister(const FName Name)
{
    if (!IsRegistered(Name))
    {
        return false;
    }
    ISubsystem* Subsystem = GetSubsystem(Name);
    Subsystem->Deinitialize();
    Subsystems.erase(Name);
    return true;
}
