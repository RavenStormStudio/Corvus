#pragma once
#include "Engine/Core/CoreDefinitions.hpp"

class ENGINE_API ISubsystem
{
public:
    ISubsystem() = default;
    virtual ~ISubsystem() = default;

    NON_COPY_MOVEABLE(ISubsystem)

public:
    virtual void Initialize() = 0;
    virtual void Deinitialize() = 0;

private:
};
