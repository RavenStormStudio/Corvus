#include "Engine/Core/Memory/Memory.hpp"

void* FMemory::Allocate(size64 Size, uint8 Alignment)
{
    return _aligned_malloc(Size, Alignment);
}

void* FMemory::Reallocate(void* OriginalPointer, size64 Size, uint8 Alignment)
{
    return _aligned_realloc(OriginalPointer, Size, Alignment);
}

void FMemory::Free(void* Pointer, [[maybe_unused]] uint8 Alignment)
{
    _aligned_free(Pointer);
}

size64 FMemory::GetPointerSize(void* Pointer, uint8 Alignment)
{
    return _aligned_msize(Pointer, Alignment, 0);
}
