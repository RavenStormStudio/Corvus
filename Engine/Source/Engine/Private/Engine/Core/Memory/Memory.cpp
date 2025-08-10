#include "Engine/Core/Memory/Memory.hpp"

void* FMemory::Allocate(const size64 Size, const uint8 Alignment)
{
    return _aligned_malloc(Size, Alignment);
}

void* FMemory::Reallocate(void* OriginalPointer, const size64 Size, const uint8 Alignment)
{
    return _aligned_realloc(OriginalPointer, Size, Alignment);
}

void FMemory::Free(void* Pointer, [[maybe_unused]] uint8 Alignment)
{
    _aligned_free(Pointer);
}

size64 FMemory::GetPointerSize(void* Pointer, const uint8 Alignment)
{
    return _aligned_msize(Pointer, Alignment, 0);
}

void* operator new(const size64 Size)
{
    return FMemory::Allocate(Size);
}

void* operator new [](const size64 Size)
{
    return FMemory::Allocate(Size);
}

void operator delete(void* Pointer) noexcept
{
    FMemory::Free(Pointer);
}

void operator delete [](void* Pointer) noexcept
{
    FMemory::Free(Pointer);
}
