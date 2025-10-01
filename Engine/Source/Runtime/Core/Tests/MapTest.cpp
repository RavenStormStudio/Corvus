// RavenStorm Copyright @ 2025-2025

#include <string>
#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include "Core/Containers/Map.hpp"

TEST_CASE("TMap::DefaultConstruction", "[Map]")
{
    const TMap<int32, int32> Map;

    REQUIRE(Map.Num() == 0);
    REQUIRE(Map.IsEmpty());
    REQUIRE(Map.GetBucketCount() >= 16);
}

TEST_CASE("TMap::ConstructionWithBucketCount", "[Map]")
{
    const TMap<int32, int32> Map(32);

    REQUIRE(Map.Num() == 0);
    REQUIRE(Map.IsEmpty());
    REQUIRE(Map.GetBucketCount() >= 32);
}

TEST_CASE("TMap::ConstructionWithInitializerList", "[Map]")
{
    TMap<int32, int32> Map{{1, 10}, {2, 20}, {3, 30}};

    REQUIRE(Map.Num() == 3);
    REQUIRE_FALSE(Map.IsEmpty());
    REQUIRE(Map.Contains(1));
    REQUIRE(Map.Contains(2));
    REQUIRE(Map.Contains(3));
}

TEST_CASE("TMap::CopyConstruction", "[Map]")
{
    TMap<int32, int32> Original{{1, 10}, {2, 20}, {3, 30}};
    TMap<int32, int32> Copy(Original);

    REQUIRE(Copy.Num() == 3);
    REQUIRE(Copy.Contains(1));
    REQUIRE(Copy.Contains(2));
    REQUIRE(Copy.Contains(3));

    Copy[1] = 999;
    REQUIRE(Original[1] == 10);
}

TEST_CASE("TMap::MoveConstruction", "[Map]")
{
    TMap<int32, int32> Original{{1, 10}, {2, 20}, {3, 30}};
    const size64 OriginalBucketCount = Original.GetBucketCount();
    TMap<int32, int32> Moved(std::move(Original));

    REQUIRE(Moved.Num() == 3);
    REQUIRE(Moved.Contains(1));
    REQUIRE(Moved.Contains(2));
    REQUIRE(Moved.Contains(3));
    REQUIRE(Moved.GetBucketCount() == OriginalBucketCount);

    REQUIRE(Original.Num() == 0);
    REQUIRE(Original.IsEmpty());
}

TEST_CASE("TMap::CopyAssignment", "[Map]")
{
    TMap<int32, int32> Source{{1, 10}, {2, 20}, {3, 30}};
    TMap<int32, int32> Destination;

    Destination = Source;

    REQUIRE(Destination.Num() == 3);
    REQUIRE(Destination.Contains(1));
    REQUIRE(Destination.Contains(2));
    REQUIRE(Destination.Contains(3));

    Destination[1] = 999;
    REQUIRE(Source[1] == 10);
}

TEST_CASE("TMap::MoveAssignment", "[Map]")
{
    TMap<int32, int32> Source{{1, 10}, {2, 20}, {3, 30}};
    const size64 SourceBucketCount = Source.GetBucketCount();
    TMap<int32, int32> Destination;

    Destination = std::move(Source);

    REQUIRE(Destination.Num() == 3);
    REQUIRE(Destination.Contains(1));
    REQUIRE(Destination.Contains(2));
    REQUIRE(Destination.Contains(3));
    REQUIRE(Destination.GetBucketCount() == SourceBucketCount);

    REQUIRE(Source.Num() == 0);
    REQUIRE(Source.IsEmpty());
}

TEST_CASE("TMap::InitializerListAssignment", "[Map]")
{
    TMap<int32, int32> Map{{1, 10}, {2, 20}};

    Map = {{3, 30}, {4, 40}, {5, 50}};

    REQUIRE(Map.Num() == 3);
    REQUIRE(Map.Contains(3));
    REQUIRE(Map.Contains(4));
    REQUIRE(Map.Contains(5));
    REQUIRE_FALSE(Map.Contains(1));
    REQUIRE_FALSE(Map.Contains(2));
}

TEST_CASE("TMap::SelfAssignment", "[Map]")
{
    TMap<int32, int32> Map{{1, 10}, {2, 20}, {3, 30}};
    const size64 OriginalSize = Map.Num();

    Map = Map;

    REQUIRE(Map.Num() == OriginalSize);
    REQUIRE(Map.Contains(1));
    REQUIRE(Map.Contains(2));
    REQUIRE(Map.Contains(3));
}

TEST_CASE("TMap::InsertLValue", "[Map]")
{
    TMap<int32, int32> Map;

    std::pair<const int32, int32> Pair1{1, 10};
    auto [Iterator1, Inserted1] = Map.Insert(Pair1);

    REQUIRE(Inserted1);
    REQUIRE(Map.Num() == 1);
    REQUIRE(Iterator1->first == 1);
    REQUIRE(Iterator1->second == 10);

    std::pair<const int32, int32> Pair2{1, 20};
    auto [Iterator2, Inserted2] = Map.Insert(Pair2);

    REQUIRE_FALSE(Inserted2);
    REQUIRE(Map.Num() == 1);
    REQUIRE(Iterator2->second == 10);
}

TEST_CASE("TMap::InsertRValue", "[Map]")
{
    TMap<int32, int32> Map;

    auto [Iterator, Inserted] = Map.Insert({1, 10});

    REQUIRE(Inserted);
    REQUIRE(Map.Num() == 1);
    REQUIRE(Iterator->first == 1);
    REQUIRE(Iterator->second == 10);
}

TEST_CASE("TMap::Emplace", "[Map]")
{
    TMap<int32, int32> Map;

    auto [Iterator, Inserted] = Map.Emplace(1, 10);

    REQUIRE(Inserted);
    REQUIRE(Map.Num() == 1);
    REQUIRE(Iterator->first == 1);
    REQUIRE(Iterator->second == 10);
}

TEST_CASE("TMap::OperatorBrackets", "[Map]")
{
    TMap<int32, int32> Map;

    Map[1] = 10;
    Map[2] = 20;
    Map[3] = 30;

    REQUIRE(Map.Num() == 3);
    REQUIRE(Map[1] == 10);
    REQUIRE(Map[2] == 20);
    REQUIRE(Map[3] == 30);

    Map[1] = 999;
    REQUIRE(Map[1] == 999);
}

TEST_CASE("TMap::Find", "[Map]")
{
    TMap<int32, int32> Map{{1, 10}, {2, 20}, {3, 30}};

    SECTION("FindExistingKey")
    {
        auto Iterator = Map.Find(2);
        REQUIRE(Iterator != Map.end());
        REQUIRE(Iterator->first == 2);
        REQUIRE(Iterator->second == 20);
    }

    SECTION("FindNonExistingKey")
    {
        auto Iterator = Map.Find(999);
        REQUIRE(Iterator == Map.end());
    }

    SECTION("ConstFind")
    {
        const TMap<int32, int32>& ConstMap = Map;
        auto Iterator = ConstMap.Find(2);
        REQUIRE(Iterator != ConstMap.end());
        REQUIRE(Iterator->first == 2);
        REQUIRE(Iterator->second == 20);
    }
}

TEST_CASE("TMap::Contains", "[Map]")
{
    TMap<int32, int32> Map{{1, 10}, {2, 20}, {3, 30}};

    REQUIRE(Map.Contains(1));
    REQUIRE(Map.Contains(2));
    REQUIRE(Map.Contains(3));
    REQUIRE_FALSE(Map.Contains(4));
    REQUIRE_FALSE(Map.Contains(999));
}

TEST_CASE("TMap::RemoveByKey", "[Map]")
{
    TMap<int32, int32> Map{{1, 10}, {2, 20}, {3, 30}};

    SECTION("RemoveExistingKey")
    {
        size64 Removed = Map.Remove(2);

        REQUIRE(Removed == 1);
        REQUIRE(Map.Num() == 2);
        REQUIRE_FALSE(Map.Contains(2));
        REQUIRE(Map.Contains(1));
        REQUIRE(Map.Contains(3));
    }

    SECTION("RemoveNonExistingKey")
    {
        size64 Removed = Map.Remove(999);

        REQUIRE(Removed == 0);
        REQUIRE(Map.Num() == 3);
    }

    SECTION("RemoveMultipleKeys")
    {
        Map.Remove(1);
        Map.Remove(2);
        Map.Remove(3);

        REQUIRE(Map.Num() == 0);
        REQUIRE(Map.IsEmpty());
    }
}

TEST_CASE("TMap::RemoveByIterator", "[Map]")
{
    TMap<int32, int32> Map{{1, 10}, {2, 20}, {3, 30}};

    auto Iterator = Map.Find(2);
    REQUIRE(Iterator != Map.end());

    auto NextIterator = Map.Remove(Iterator);

    REQUIRE(Map.Num() == 2);
    REQUIRE_FALSE(Map.Contains(2));
    REQUIRE(Map.Contains(1));
    REQUIRE(Map.Contains(3));
}

TEST_CASE("TMap::Clear", "[Map]")
{
    TMap<int32, int32> Map{{1, 10}, {2, 20}, {3, 30}};
    const size64 OriginalBucketCount = Map.GetBucketCount();

    Map.Clear();

    REQUIRE(Map.Num() == 0);
    REQUIRE(Map.IsEmpty());
    REQUIRE(Map.GetBucketCount() == OriginalBucketCount);

    Map[1] = 100;
    REQUIRE(Map.Num() == 1);
    REQUIRE(Map[1] == 100);
}

TEST_CASE("TMap::Reserve", "[Map]")
{
    TMap<int32, int32> Map;
    const size64 InitialBucketCount = Map.GetBucketCount();

    Map.Reserve(100);

    REQUIRE(Map.GetBucketCount() > InitialBucketCount);
    REQUIRE(Map.Num() == 0);

    for (int32 Index = 0; Index < 100; ++Index)
    {
        Map[Index] = Index * 10;
    }

    REQUIRE(Map.Num() == 100);
}

TEST_CASE("TMap::Swap", "[Map]")
{
    TMap<int32, int32> Map1{{1, 10}, {2, 20}};
    TMap<int32, int32> Map2{{3, 30}, {4, 40}, {5, 50}};

    Map1.Swap(Map2);

    REQUIRE(Map1.Num() == 3);
    REQUIRE(Map1.Contains(3));
    REQUIRE(Map1.Contains(4));
    REQUIRE(Map1.Contains(5));

    REQUIRE(Map2.Num() == 2);
    REQUIRE(Map2.Contains(1));
    REQUIRE(Map2.Contains(2));
}

TEST_CASE("TMap::IteratorSupport", "[Map]")
{
    TMap<int32, int32> Map{{1, 10}, {2, 20}, {3, 30}};

    SECTION("BeginEnd")
    {
        int32 Count = 0;
        for (auto Iterator = Map.begin(); Iterator != Map.end(); ++Iterator)
        {
            ++Count;
        }
        REQUIRE(Count == 3);
    }

    SECTION("ConstBeginEnd")
    {
        const TMap<int32, int32>& ConstMap = Map;
        int32 Count = 0;
        for (auto Iterator = ConstMap.begin(); Iterator != ConstMap.end(); ++Iterator)
        {
            ++Count;
        }
        REQUIRE(Count == 3);
    }

    SECTION("CBeginCEnd")
    {
        int32 Count = 0;
        for (auto Iterator = Map.cbegin(); Iterator != Map.cend(); ++Iterator)
        {
            ++Count;
        }
        REQUIRE(Count == 3);
    }

    SECTION("RangeBasedForLoop")
    {
        int32 Sum = 0;
        for (const auto& [Key, Value] : Map)
        {
            Sum += Value;
        }
        REQUIRE(Sum == 60);
    }

    SECTION("RangeBasedForLoopModification")
    {
        for (auto& [Key, Value] : Map)
        {
            Value *= 2;
        }

        REQUIRE(Map[1] == 20);
        REQUIRE(Map[2] == 40);
        REQUIRE(Map[3] == 60);
    }
}

TEST_CASE("TMap::QueryMethods", "[Map]")
{
    TMap<int32, int32> Map{{1, 10}, {2, 20}, {3, 30}};

    SECTION("Num")
    {
        REQUIRE(Map.Num() == 3);
    }

    SECTION("GetBucketCount")
    {
        REQUIRE(Map.GetBucketCount() >= 3);
    }

    SECTION("IsEmpty")
    {
        REQUIRE_FALSE(Map.IsEmpty());

        TMap<int32, int32> EmptyMap;
        REQUIRE(EmptyMap.IsEmpty());
    }

    SECTION("GetLoadFactor")
    {
        float64 LoadFactor = Map.GetLoadFactor();
        REQUIRE(LoadFactor > 0.0);
        REQUIRE(LoadFactor <= TMap<int32, int32>::GetMaxLoadFactor());
    }

    SECTION("GetMaxLoadFactor")
    {
        REQUIRE(TMap<int32, int32>::GetMaxLoadFactor() == 0.75);
    }
}

TEST_CASE("TMap::StringKeys", "[Map]")
{
    TMap<std::string, int32> Map{{"one", 1}, {"two", 2}, {"three", 3}};

    REQUIRE(Map.Num() == 3);
    REQUIRE(Map["one"] == 1);
    REQUIRE(Map["two"] == 2);
    REQUIRE(Map["three"] == 3);

    Map["four"] = 4;
    REQUIRE(Map.Num() == 4);
    REQUIRE(Map["four"] == 4);
}

TEST_CASE("TMap::AutomaticRehashing", "[Map]")
{
    TMap<int32, int32> Map;
    const size64 InitialBucketCount = Map.GetBucketCount();

    for (int32 Index = 0; Index < 100; ++Index)
    {
        Map[Index] = Index * 10;
    }

    REQUIRE(Map.Num() == 100);
    REQUIRE(Map.GetBucketCount() > InitialBucketCount);
    REQUIRE(Map.GetLoadFactor() <= TMap<int32, int32>::GetMaxLoadFactor());
}

TEST_CASE("TMap::CollisionHandling", "[Map]")
{
    TMap<int32, int32> Map(1);

    Map[1] = 10;
    Map[2] = 20;
    Map[3] = 30;

    REQUIRE(Map.Num() == 3);
    REQUIRE(Map[1] == 10);
    REQUIRE(Map[2] == 20);
    REQUIRE(Map[3] == 30);
}

TEST_CASE("TMap::DeductionGuide", "[Map]")
{
    TMap<uint32, uint32> Map{{1, 10}, {2, 20}, {3, 30}};

    REQUIRE(Map.Num() == 3);
    REQUIRE(Map[1] == 10);
    REQUIRE(Map[3] == 30);
}

TEST_CASE("TMap::LargeDataset", "[Map]")
{
    TMap<int32, int32> Map;

    for (int32 Index = 0; Index < 1000; ++Index)
    {
        Map[Index] = Index * 10;
    }

    REQUIRE(Map.Num() == 1000);

    for (int32 Index = 0; Index < 1000; ++Index)
    {
        REQUIRE(Map.Contains(Index));
        REQUIRE(Map[Index] == Index * 10);
    }
}

TEST_CASE("TMap::InsertAndRemovePattern", "[Map]")
{
    TMap<int32, int32> Map;

    for (int32 Index = 0; Index < 50; ++Index)
    {
        Map[Index] = Index;
    }
    REQUIRE(Map.Num() == 50);

    for (int32 Index = 0; Index < 25; ++Index)
    {
        Map.Remove(Index);
    }
    REQUIRE(Map.Num() == 25);

    for (int32 Index = 0; Index < 25; ++Index)
    {
        REQUIRE_FALSE(Map.Contains(Index));
    }

    for (int32 Index = 25; Index < 50; ++Index)
    {
        REQUIRE(Map.Contains(Index));
    }
}

TEST_CASE("TMap::BenchmarkConstruction", "[Map][.benchmark]")
{
    BENCHMARK("DefaultConstruction")
    {
        const TMap<int32, int32> Map;
        return Map.Num();
    };

    BENCHMARK("ReserveConstruction")
    {
        TMap<int32, int32> Map;
        Map.Reserve(100);
        return Map.GetBucketCount();
    };

    BENCHMARK("InitializerListConstruction")
    {
        const TMap<int32, int32> Map{{1, 10}, {2, 20}, {3, 30}, {4, 40}, {5, 50}};
        return Map.Num();
    };

    BENCHMARK("CopyConstruction")
    {
        const TMap<int32, int32> Original{{1, 10}, {2, 20}, {3, 30}, {4, 40}, {5, 50}};
        const TMap<int32, int32> Copy(Original);
        return Copy.Num();
    };

    BENCHMARK("MoveConstruction")
    {
        TMap<int32, int32> Original{{1, 10}, {2, 20}, {3, 30}, {4, 40}, {5, 50}};
        const TMap<int32, int32> Moved(std::move(Original));
        return Moved.Num();
    };
}

TEST_CASE("TMap::BenchmarkInsertion", "[Map][.benchmark]")
{
    BENCHMARK("Insert_NoReserve")
    {
        TMap<int32, int32> Map;
        for (int32 Index = 0; Index < 100; ++Index)
        {
            Map.Insert({Index, Index * 10});
        }
        return Map.Num();
    };

    BENCHMARK("Insert_WithReserve")
    {
        TMap<int32, int32> Map;
        Map.Reserve(100);
        for (int32 Index = 0; Index < 100; ++Index)
        {
            Map.Insert({Index, Index * 10});
        }
        return Map.Num();
    };

    BENCHMARK("OperatorBrackets")
    {
        TMap<int32, int32> Map;
        for (int32 Index = 0; Index < 100; ++Index)
        {
            Map[Index] = Index * 10;
        }
        return Map.Num();
    };

    BENCHMARK("Emplace")
    {
        TMap<int32, int32> Map;
        Map.Reserve(100);
        for (int32 Index = 0; Index < 100; ++Index)
        {
            Map.Emplace(Index, Index * 10);
        }
        return Map.Num();
    };
}

TEST_CASE("TMap::BenchmarkLookup", "[Map][.benchmark]")
{
    TMap<int32, int32> Map;
    for (int32 Index = 0; Index < 1000; ++Index)
    {
        Map[Index] = Index * 10;
    }

    BENCHMARK("Find")
    {
        int32 Sum = 0;
        for (int32 Index = 0; Index < 100; ++Index)
        {
            auto Iterator = Map.Find(Index);
            if (Iterator != Map.end())
            {
                Sum += Iterator->second;
            }
        }
        return Sum;
    };

    BENCHMARK("Contains")
    {
        int32 Count = 0;
        for (int32 Index = 0; Index < 100; ++Index)
        {
            if (Map.Contains(Index))
            {
                ++Count;
            }
        }
        return Count;
    };

    BENCHMARK("OperatorBrackets")
    {
        int32 Sum = 0;
        for (int32 Index = 0; Index < 100; ++Index)
        {
            Sum += Map[Index];
        }
        return Sum;
    };
}

TEST_CASE("TMap::BenchmarkIteration", "[Map][.benchmark]")
{
    TMap<int32, int32> Map;
    for (int32 Index = 0; Index < 1000; ++Index)
    {
        Map[Index] = Index * 10;
    }

    BENCHMARK("RangeBasedForLoopRead")
    {
        int32 Sum = 0;
        for (const auto& [Key, Value] : Map)
        {
            Sum += Value;
        }
        return Sum;
    };

    BENCHMARK("RangeBasedForLoopWrite")
    {
        for (auto& [Key, Value] : Map)
        {
            Value += 1;
        }
        return Map[0];
    };

    BENCHMARK("IteratorIteration")
    {
        int32 Sum = 0;
        for (auto Iterator = Map.begin(); Iterator != Map.end(); ++Iterator)
        {
            Sum += Iterator->second;
        }
        return Sum;
    };
}

TEST_CASE("TMap::BenchmarkRemoval", "[Map][.benchmark]")
{
    BENCHMARK("RemoveByKey")
    {
        TMap<int32, int32> Map;
        for (int32 Index = 0; Index < 100; ++Index)
        {
            Map[Index] = Index * 10;
        }
        for (int32 Index = 0; Index < 50; ++Index)
        {
            Map.Remove(Index);
        }
        return Map.Num();
    };

    BENCHMARK("Clear")
    {
        TMap<int32, int32> Map;
        for (int32 Index = 0; Index < 1000; ++Index)
        {
            Map[Index] = Index * 10;
        }
        Map.Clear();
        return Map.Num();
    };
}

TEST_CASE("TMap::BenchmarkOperations", "[Map][.benchmark]")
{
    BENCHMARK("Reserve")
    {
        TMap<int32, int32> Map;
        Map.Reserve(1000);
        return Map.GetBucketCount();
    };

    BENCHMARK("Swap")
    {
        TMap<int32, int32> Map1;
        TMap<int32, int32> Map2;
        for (int32 Index = 0; Index < 100; ++Index)
        {
            Map1[Index] = Index;
            Map2[Index + 100] = Index + 100;
        }
        Map1.Swap(Map2);
        return Map1.Num();
    };
}

TEST_CASE("TMap::BenchmarkStringKeys", "[Map][.benchmark]")
{
    BENCHMARK("StringInsertion")
    {
        TMap<std::string, int32> Map;
        for (int32 Index = 0; Index < 100; ++Index)
        {
            Map[std::to_string(Index)] = Index;
        }
        return Map.Num();
    };

    BENCHMARK("StringLookup")
    {
        TMap<std::string, int32> Map;
        for (int32 Index = 0; Index < 100; ++Index)
        {
            Map[std::to_string(Index)] = Index;
        }
        int32 Sum = 0;
        for (int32 Index = 0; Index < 100; ++Index)
        {
            Sum += Map[std::to_string(Index)];
        }
        return Sum;
    };
}
