// RavenStorm Copyright @ 2025-2025

#include "Core/Memory/Memory.hpp"

#include "mimalloc.h"

void* FMemory::Allocate(const size64 Size, const uint8 Alignment)
{
    return mi_malloc_aligned(Size, Alignment);
}

void* FMemory::Reallocate(void* OldPointer, const size64 Size, const uint8 Alignment)
{
    return mi_realloc_aligned(OldPointer, Size, Alignment);
}

void FMemory::Free(void* OldPointer, [[maybe_unused]] const uint8 Alignment)
{
    mi_free_aligned(OldPointer, Alignment);
}

void FMemory::Copy(const void* SourcePointer, void* TargetPointer, const size64 Size)
{
    std::memcpy(TargetPointer, SourcePointer, Size);
}

size64 FMemory::GetAllocationSize(const void* Pointer)
{
    return mi_usable_size(Pointer);
}
