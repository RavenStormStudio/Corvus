#include <iostream>

#include <catch2/catch_all.hpp>

#include "Engine/Core/CoreTypes.hpp"
#include "Engine/Core/Memory/Memory.hpp"

TEST_CASE("Should allocate", "Memory")
{
    size64 Size = 200;

    void* Pointer = FMemory::Allocate(Size);
    size64 PointerSize = FMemory::GetPointerSize(Pointer);

    REQUIRE(Pointer != nullptr);
    REQUIRE(PointerSize == Size);
    
    FMemory::Free(Pointer);
}

TEST_CASE("Memory Benchmarks", "Memory")
{
    SECTION("Allocate")
    {
        BENCHMARK("Allocate 1KB")
        {
            size64 Size = 1024;
            void* Pointer = FMemory::Allocate(Size);
            FMemory::Free(Pointer);
        };

        BENCHMARK("Allocate 16KB")
        {
            size64 Size = 16384;
            void* Pointer = FMemory::Allocate(Size);
            FMemory::Free(Pointer);
        };

        BENCHMARK("Allocate 1MB")
        {
            size64 Size = 1048576;
            void* Pointer = FMemory::Allocate(Size);
            FMemory::Free(Pointer);
        };

        BENCHMARK("Allocate 10MB")
        {
            size64 Size = 10485760;
            void* Pointer = FMemory::Allocate(Size);
            FMemory::Free(Pointer);
        };
    }
}