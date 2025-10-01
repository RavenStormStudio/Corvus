// RavenStorm Copyright @ 2025-2025

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include "Core/Containers/Array.hpp"

struct FTestStruct
{
    int32 Value;
    float32 Score;

    constexpr bool operator==(const FTestStruct& Other) const noexcept
    {
        return Value == Other.Value && Score == Other.Score;
    }

    constexpr std::partial_ordering operator<=>(const FTestStruct& Other) const noexcept
    {
        if (const auto Result = Value <=> Other.Value; Result != 0)
        {
            return Result;
        }
        if (Score < Other.Score) return std::partial_ordering::less;
        if (Score > Other.Score) return std::partial_ordering::greater;
        return std::partial_ordering::equivalent;
    }
};

struct FSmallArrayFixture
{
    TArray<int32> SmallArray;

    FSmallArrayFixture()
    {
        SmallArray = {1, 2, 3, 4, 5};
    }
};

struct FMediumArrayFixture
{
    TArray<int32> MediumArray;

    FMediumArrayFixture()
    {
        MediumArray.Reserve(100);
        for (int32 Index = 0; Index < 100; ++Index)
        {
            MediumArray.PushBack(42);
        }
    }
};

struct FLargeArrayFixture
{
    TArray<int32> LargeArray;

    FLargeArrayFixture()
    {
        LargeArray.Reserve(1000);
        for (int32 Index = 0; Index < 1000; ++Index)
        {
            LargeArray.PushBack(42);
        }
    }
};

struct FComparisonArraysFixture
{
    TArray<int32> Array1{1, 2, 3};
    TArray<int32> Array2{1, 2, 3};
    TArray<int32> Array3{1, 2, 4};
    TArray<int32> Array4{1, 2, 2};
};

struct FStructArrayFixture
{
    TArray<FTestStruct> StructArray;

    FStructArrayFixture()
    {
        StructArray = {
            FTestStruct{.Value = 1, .Score = 1.0f},
            FTestStruct{.Value = 2, .Score = 2.0f},
            FTestStruct{.Value = 3, .Score = 3.0f}
        };
    }
};

TEST_CASE("TArray::DefaultConstruction", "[Array]")
{
    TArray<int32> Array;

    REQUIRE(Array.Num() == 0);
    REQUIRE(Array.GetCapacity() == 0);
    REQUIRE(Array.IsEmpty());
    REQUIRE(Array.GetData() == nullptr);
}

TEST_CASE("TArray::ConstructionWithCount", "[Array]")
{
    TArray<int32> Array(5);

    REQUIRE(Array.Num() == 5);
    REQUIRE(Array.GetCapacity() >= 5);
    REQUIRE_FALSE(Array.IsEmpty());
}

TEST_CASE("TArray::ConstructionWithCountAndValue", "[Array]")
{
    TArray<int32> Array(5, 42);

    REQUIRE(Array.Num() == 5);
    REQUIRE(Array[0] == 42);
    REQUIRE(Array[1] == 42);
    REQUIRE(Array[2] == 42);
    REQUIRE(Array[3] == 42);
    REQUIRE(Array[4] == 42);
}

TEST_CASE("TArray::ConstructionWithInitializerList", "[Array]")
{
    TArray<int32> Array{1, 2, 3, 4, 5};

    REQUIRE(Array.Num() == 5);
    REQUIRE(Array[0] == 1);
    REQUIRE(Array[1] == 2);
    REQUIRE(Array[2] == 3);
    REQUIRE(Array[3] == 4);
    REQUIRE(Array[4] == 5);
}

TEST_CASE("TArray::CopyConstruction", "[Array]")
{
    TArray<int32> Original{100, 200, 300};
    TArray<int32> Copy(Original);

    REQUIRE(Copy.Num() == 3);
    REQUIRE(Copy[0] == 100);
    REQUIRE(Copy[1] == 200);
    REQUIRE(Copy[2] == 300);

    Copy[0] = 999;
    REQUIRE(Original[0] == 100);
}

TEST_CASE("TArray::MoveConstruction", "[Array]")
{
    TArray<int32> Original{100, 200, 300};
    const size64 OriginalCapacity = Original.GetCapacity();
    TArray<int32> Moved(std::move(Original));

    REQUIRE(Moved.Num() == 3);
    REQUIRE(Moved[0] == 100);
    REQUIRE(Moved[1] == 200);
    REQUIRE(Moved[2] == 300);
    REQUIRE(Moved.GetCapacity() == OriginalCapacity);

    REQUIRE(Original.Num() == 0);
    REQUIRE(Original.GetCapacity() == 0);
    REQUIRE(Original.IsEmpty());
}

TEST_CASE("TArray::CopyAssignment", "[Array]")
{
    TArray<int32> Source{10, 20, 30};
    TArray<int32> Destination;

    Destination = Source;

    REQUIRE(Destination.Num() == 3);
    REQUIRE(Destination[0] == 10);
    REQUIRE(Destination[1] == 20);
    REQUIRE(Destination[2] == 30);

    Destination[0] = 999;
    REQUIRE(Source[0] == 10);
}

TEST_CASE("TArray::MoveAssignment", "[Array]")
{
    TArray<int32> Source{10, 20, 30};
    const size64 SourceCapacity = Source.GetCapacity();
    TArray<int32> Destination;

    Destination = std::move(Source);

    REQUIRE(Destination.Num() == 3);
    REQUIRE(Destination[0] == 10);
    REQUIRE(Destination[1] == 20);
    REQUIRE(Destination[2] == 30);
    REQUIRE(Destination.GetCapacity() == SourceCapacity);

    REQUIRE(Source.Num() == 0);
    REQUIRE(Source.GetCapacity() == 0);
}

TEST_CASE("TArray::InitializerListAssignment", "[Array]")
{
    TArray<int32> Array{1, 2, 3};

    Array = {10, 20, 30, 40};

    REQUIRE(Array.Num() == 4);
    REQUIRE(Array[0] == 10);
    REQUIRE(Array[1] == 20);
    REQUIRE(Array[2] == 30);
    REQUIRE(Array[3] == 40);
}

TEST_CASE("TArray::SelfAssignment", "[Array]")
{
    TArray<int32> Array{1, 2, 3};
    const size64 OriginalSize = Array.Num();

    Array = Array;

    REQUIRE(Array.Num() == OriginalSize);
    REQUIRE(Array[0] == 1);
    REQUIRE(Array[1] == 2);
    REQUIRE(Array[2] == 3);
}

TEST_CASE_METHOD(FSmallArrayFixture, "TArray::ElementAccess", "[Array]")
{
    SECTION("OperatorBrackets")
    {
        REQUIRE(SmallArray[0] == 1);
        REQUIRE(SmallArray[4] == 5);

        SmallArray[2] = 99;
        REQUIRE(SmallArray[2] == 99);
    }

    SECTION("ConstOperatorBrackets")
    {
        const TArray<int32>& ConstArray = SmallArray;
        REQUIRE(ConstArray[0] == 1);
        REQUIRE(ConstArray[4] == 5);
    }

    SECTION("AtMethod")
    {
        REQUIRE(SmallArray.At(0) == 1);
        REQUIRE(SmallArray.At(4) == 5);

        SmallArray.At(3) = 88;
        REQUIRE(SmallArray.At(3) == 88);
    }

    SECTION("ConstAtMethod")
    {
        const TArray<int32>& ConstArray = SmallArray;
        REQUIRE(ConstArray.At(0) == 1);
        REQUIRE(ConstArray.At(4) == 5);
    }
}

TEST_CASE_METHOD(FSmallArrayFixture, "TArray::GetFirstAndGetLast", "[Array]")
{
    SECTION("GetFirst")
    {
        REQUIRE(SmallArray.GetFirst() == 1);

        SmallArray.GetFirst() = 100;
        REQUIRE(SmallArray.GetFirst() == 100);
    }

    SECTION("GetLast")
    {
        REQUIRE(SmallArray.GetLast() == 5);

        SmallArray.GetLast() = 500;
        REQUIRE(SmallArray.GetLast() == 500);
    }

    SECTION("ConstGetFirst")
    {
        const TArray<int32>& ConstArray = SmallArray;
        REQUIRE(ConstArray.GetFirst() == 1);
    }

    SECTION("ConstGetLast")
    {
        const TArray<int32>& ConstArray = SmallArray;
        REQUIRE(ConstArray.GetLast() == 5);
    }
}

TEST_CASE_METHOD(FSmallArrayFixture, "TArray::GetData", "[Array]")
{
    SECTION("NonConstGetData")
    {
        int32* DataPtr = SmallArray.GetData();
        REQUIRE(DataPtr != nullptr);
        REQUIRE(DataPtr[0] == 1);
        REQUIRE(DataPtr[4] == 5);

        DataPtr[2] = 99;
        REQUIRE(SmallArray[2] == 99);
    }

    SECTION("ConstGetData")
    {
        const TArray<int32>& ConstArray = SmallArray;
        const int32* DataPtr = ConstArray.GetData();
        REQUIRE(DataPtr != nullptr);
        REQUIRE(DataPtr[0] == 1);
        REQUIRE(DataPtr[4] == 5);
    }
}

TEST_CASE_METHOD(FSmallArrayFixture, "TArray::PushBack", "[Array]")
{
    SECTION("PushBackLValue")
    {
        int32 Value = 42;
        SmallArray.PushBack(Value);

        REQUIRE(SmallArray.Num() == 6);
        REQUIRE(SmallArray.GetLast() == 42);
    }

    SECTION("PushBackRValue")
    {
        SmallArray.PushBack(99);

        REQUIRE(SmallArray.Num() == 6);
        REQUIRE(SmallArray.GetLast() == 99);
    }

    SECTION("PushBackMultiple")
    {
        SmallArray.PushBack(10);
        SmallArray.PushBack(20);
        SmallArray.PushBack(30);

        REQUIRE(SmallArray.Num() == 8);
        REQUIRE(SmallArray[5] == 10);
        REQUIRE(SmallArray[6] == 20);
        REQUIRE(SmallArray[7] == 30);
    }
}

TEST_CASE_METHOD(FSmallArrayFixture, "TArray::EmplaceBack", "[Array]")
{
    TArray<FTestStruct> StructArray;

    FTestStruct& Emplaced = StructArray.EmplaceBack(42, 3.14f);

    REQUIRE(StructArray.Num() == 1);
    REQUIRE(Emplaced.Value == 42);
    REQUIRE(Emplaced.Score == 3.14f);
    REQUIRE(StructArray[0].Value == 42);
}

TEST_CASE_METHOD(FSmallArrayFixture, "TArray::PopBack", "[Array]")
{
    const size64 OriginalSize = SmallArray.Num();

    SmallArray.PopBack();

    REQUIRE(SmallArray.Num() == OriginalSize - 1);
    REQUIRE(SmallArray.GetLast() == 4);

    SmallArray.PopBack();
    SmallArray.PopBack();

    REQUIRE(SmallArray.Num() == 2);
    REQUIRE(SmallArray.GetLast() == 2);
}

TEST_CASE("TArray::Reserve", "[Array]")
{
    TArray<int32> Array;

    Array.Reserve(100);

    REQUIRE(Array.GetCapacity() >= 100);
    REQUIRE(Array.Num() == 0);

    Array.PushBack(42);
    REQUIRE(Array.Num() == 1);
    REQUIRE(Array.GetCapacity() >= 100);
}

TEST_CASE("TArray::Resize", "[Array]")
{
    SECTION("ResizeGrow")
    {
        TArray<int32> Array{1, 2, 3};

        Array.Resize(5);

        REQUIRE(Array.Num() == 5);
        REQUIRE(Array[0] == 1);
        REQUIRE(Array[1] == 2);
        REQUIRE(Array[2] == 3);
    }

    SECTION("ResizeShrink")
    {
        TArray<int32> Array{1, 2, 3, 4, 5};

        Array.Resize(3);

        REQUIRE(Array.Num() == 3);
        REQUIRE(Array[0] == 1);
        REQUIRE(Array[1] == 2);
        REQUIRE(Array[2] == 3);
    }

    SECTION("ResizeToZero")
    {
        TArray<int32> Array{1, 2, 3};

        Array.Resize(0);

        REQUIRE(Array.Num() == 0);
        REQUIRE(Array.IsEmpty());
    }
}

TEST_CASE("TArray::ShrinkToFit", "[Array]")
{
    SECTION("ShrinkNonEmptyArray")
    {
        TArray<int32> Array;
        Array.Reserve(100);
        Array.PushBack(1);
        Array.PushBack(2);
        Array.PushBack(3);

        REQUIRE(Array.GetCapacity() >= 100);

        Array.ShrinkToFit();

        REQUIRE(Array.GetCapacity() == 3);
        REQUIRE(Array.Num() == 3);
        REQUIRE(Array[0] == 1);
        REQUIRE(Array[2] == 3);
    }

    SECTION("ShrinkEmptyArray")
    {
        TArray<int32> Array;
        Array.Reserve(50);
        Array.ShrinkToFit();

        REQUIRE(Array.GetCapacity() == 0);
        REQUIRE(Array.Num() == 0);
    }
}

TEST_CASE_METHOD(FSmallArrayFixture, "TArray::Clear", "[Array]")
{
    const size64 OriginalCapacity = SmallArray.GetCapacity();

    SmallArray.Clear();

    REQUIRE(SmallArray.Num() == 0);
    REQUIRE(SmallArray.IsEmpty());
    REQUIRE(SmallArray.GetCapacity() == OriginalCapacity);
}

TEST_CASE_METHOD(FSmallArrayFixture, "TArray::IteratorSupport", "[Array]")
{
    SECTION("BeginEnd")
    {
        int32* Begin = SmallArray.begin();
        int32* End = SmallArray.end();

        REQUIRE(End - Begin == 5);
        REQUIRE(*Begin == 1);
        REQUIRE(*(End - 1) == 5);
    }

    SECTION("ConstBeginEnd")
    {
        const TArray<int32>& ConstArray = SmallArray;
        const int32* Begin = ConstArray.begin();
        const int32* End = ConstArray.end();

        REQUIRE(End - Begin == 5);
        REQUIRE(*Begin == 1);
    }

    SECTION("CBeginCEnd")
    {
        const int32* Begin = SmallArray.cbegin();
        const int32* End = SmallArray.cend();

        REQUIRE(End - Begin == 5);
        REQUIRE(*Begin == 1);
    }

    SECTION("RangeBasedForLoop")
    {
        int32 Sum = 0;
        for (const int32& Element : SmallArray)
        {
            Sum += Element;
        }
        REQUIRE(Sum == 15);
    }

    SECTION("RangeBasedForLoopModification")
    {
        for (int32& Element : SmallArray)
        {
            Element *= 2;
        }

        REQUIRE(SmallArray[0] == 2);
        REQUIRE(SmallArray[1] == 4);
        REQUIRE(SmallArray[2] == 6);
        REQUIRE(SmallArray[3] == 8);
        REQUIRE(SmallArray[4] == 10);
    }
}

TEST_CASE_METHOD(FComparisonArraysFixture, "TArray::EqualityComparison", "[Array]")
{
    SECTION("Equality")
    {
        REQUIRE(Array1 == Array2);
        REQUIRE_FALSE(Array1 == Array3);
    }

    SECTION("DifferentSizes")
    {
        TArray<int32> Array5{1, 2, 3, 4};
        REQUIRE_FALSE(Array1 == Array5);
    }
}

TEST_CASE_METHOD(FComparisonArraysFixture, "TArray::ThreeWayComparison", "[Array]")
{
    SECTION("Equal")
    {
        REQUIRE((Array1 <=> Array2) == std::strong_ordering::equal);
    }

    SECTION("Less")
    {
        REQUIRE((Array1 <=> Array3) == std::strong_ordering::less);
    }

    SECTION("Greater")
    {
        REQUIRE((Array1 <=> Array4) == std::strong_ordering::greater);
    }

    SECTION("DifferentSizes")
    {
        TArray<int32> LongerArray{1, 2, 3, 4};
        REQUIRE((Array1 <=> LongerArray) == std::strong_ordering::less);
    }
}

TEST_CASE("TArray::QueryMethods", "[Array]")
{
    TArray<int32> Array{1, 2, 3, 4, 5};

    SECTION("Num")
    {
        REQUIRE(Array.Num() == 5);
    }

    SECTION("GetCapacity")
    {
        REQUIRE(Array.GetCapacity() >= 5);
    }

    SECTION("GetSizeInBytes")
    {
        REQUIRE(Array.GetSizeInBytes() == 5 * sizeof(int32));
    }

    SECTION("GetCapacityInBytes")
    {
        REQUIRE(Array.GetCapacityInBytes() >= 5 * sizeof(int32));
    }

    SECTION("IsEmpty")
    {
        REQUIRE_FALSE(Array.IsEmpty());

        TArray<int32> EmptyArray;
        REQUIRE(EmptyArray.IsEmpty());
    }
}

TEST_CASE("TArray::DeductionGuide", "[Array]")
{
    TArray Array{1, 2, 3, 4, 5};

    REQUIRE(Array.Num() == 5);
    REQUIRE(Array[0] == 1);
    REQUIRE(Array[4] == 5);
}

TEST_CASE("TArray::FloatTypes", "[Array]")
{
    TArray<float32> FloatArray{1.5f, 2.5f, 3.5f};

    REQUIRE(FloatArray.Num() == 3);
    REQUIRE(FloatArray[0] == 1.5f);
    REQUIRE(FloatArray[1] == 2.5f);
    REQUIRE(FloatArray[2] == 3.5f);

    FloatArray.PushBack(9.9f);
    REQUIRE(FloatArray.Num() == 4);
    REQUIRE(FloatArray[3] == 9.9f);
}

TEST_CASE_METHOD(FStructArrayFixture, "TArray::CustomStructType", "[Array]")
{
    SECTION("Construction")
    {
        REQUIRE(StructArray.Num() == 3);
        REQUIRE(StructArray[0].Value == 1);
        REQUIRE(StructArray[0].Score == 1.0f);
        REQUIRE(StructArray[2].Value == 3);
    }

    SECTION("PushBack")
    {
        StructArray.PushBack(FTestStruct{.Value = 4, .Score = 4.0f});

        REQUIRE(StructArray.Num() == 4);
        REQUIRE(StructArray[3].Value == 4);
        REQUIRE(StructArray[3].Score == 4.0f);
    }

    SECTION("Comparison")
    {
        TArray<FTestStruct> Array2{
            FTestStruct{.Value = 1, .Score = 1.0f},
            FTestStruct{.Value = 2, .Score = 2.0f},
            FTestStruct{.Value = 3, .Score = 3.0f}
        };

        REQUIRE(StructArray == Array2);
    }
}

TEST_CASE("TArray::CapacityGrowth", "[Array]")
{
    TArray<int32> Array;

    REQUIRE(Array.GetCapacity() == 0);

    Array.PushBack(1);
    const size64 FirstCapacity = Array.GetCapacity();
    REQUIRE(FirstCapacity >= 1);

    for (int32 Index = 0; Index < 100; ++Index)
    {
        Array.PushBack(Index);
    }

    REQUIRE(Array.Num() == 101);
    REQUIRE(Array.GetCapacity() > FirstCapacity);
}

TEST_CASE("TArray::ClearAndReuse", "[Array]")
{
    TArray<int32> Array{1, 2, 3, 4, 5};
    const size64 OriginalCapacity = Array.GetCapacity();

    Array.Clear();
    REQUIRE(Array.IsEmpty());
    REQUIRE(Array.GetCapacity() == OriginalCapacity);

    Array.PushBack(10);
    Array.PushBack(20);

    REQUIRE(Array.Num() == 2);
    REQUIRE(Array[0] == 10);
    REQUIRE(Array[1] == 20);
}

TEST_CASE("TArray::BenchmarkConstruction", "[Array][.benchmark]")
{
    BENCHMARK("DefaultConstruction")
    {
        TArray<int32> Array;
        return Array.Num();
    };

    BENCHMARK("ReserveConstruction")
    {
        TArray<int32> Array;
        Array.Reserve(100);
        return Array.GetCapacity();
    };

    BENCHMARK("InitializerListConstruction")
    {
        TArray<int32> Array{1, 2, 3, 4, 5};
        return Array.Num();
    };

    BENCHMARK("CopyConstruction")
    {
        const TArray<int32> Original{1, 2, 3, 4, 5};
        TArray<int32> Copy(Original);
        return Copy.Num();
    };

    BENCHMARK("MoveConstruction")
    {
        TArray<int32> Original{1, 2, 3, 4, 5};
        TArray<int32> Moved(std::move(Original));
        return Moved.Num();
    };
}

TEST_CASE_METHOD(FLargeArrayFixture, "TArray::BenchmarkElementAccess", "[Array][.benchmark]")
{
    BENCHMARK("SequentialRead")
    {
        int32 Sum = 0;
        for (size64 Index = 0; Index < LargeArray.Num(); ++Index)
        {
            Sum += LargeArray[Index];
        }
        return Sum;
    };

    BENCHMARK("SequentialWrite")
    {
        for (size64 Index = 0; Index < LargeArray.Num(); ++Index)
        {
            LargeArray[Index] = static_cast<int32>(Index);
        }
        return LargeArray[0];
    };

    BENCHMARK("RandomAccess")
    {
        int32 Sum = 0;
        Sum += LargeArray[0];
        Sum += LargeArray[500];
        Sum += LargeArray[999];
        return Sum;
    };
}

TEST_CASE_METHOD(FLargeArrayFixture, "TArray::BenchmarkIteration", "[Array][.benchmark]")
{
    BENCHMARK("RangeBasedForLoopRead")
    {
        int32 Sum = 0;
        for (const int32& Element : LargeArray)
        {
            Sum += Element;
        }
        return Sum;
    };

    BENCHMARK("RangeBasedForLoopWrite")
    {
        for (int32& Element : LargeArray)
        {
            Element += 1;
        }
        return LargeArray[0];
    };

    BENCHMARK("PointerIteration")
    {
        int32 Sum = 0;
        const int32* Begin = LargeArray.begin();
        const int32* End = LargeArray.end();
        for (const int32* Ptr = Begin; Ptr != End; ++Ptr)
        {
            Sum += *Ptr;
        }
        return Sum;
    };
}

TEST_CASE("TArray::BenchmarkPushBack", "[Array][.benchmark]")
{
    BENCHMARK("PushBack_NoReserve")
    {
        TArray<int32> Array;
        for (int32 Index = 0; Index < 100; ++Index)
        {
            Array.PushBack(Index);
        }
        return Array.Num();
    };

    BENCHMARK("PushBack_WithReserve")
    {
        TArray<int32> Array;
        Array.Reserve(100);
        for (int32 Index = 0; Index < 100; ++Index)
        {
            Array.PushBack(Index);
        }
        return Array.Num();
    };

    BENCHMARK("EmplaceBack")
    {
        TArray<FTestStruct> Array;
        Array.Reserve(100);
        for (int32 Index = 0; Index < 100; ++Index)
        {
            Array.EmplaceBack(Index, static_cast<float32>(Index));
        }
        return Array.Num();
    };
}

TEST_CASE("TArray::BenchmarkOperations", "[Array][.benchmark]")
{
    BENCHMARK("Reserve")
    {
        TArray<int32> Array;
        Array.Reserve(1000);
        return Array.GetCapacity();
    };

    BENCHMARK("Resize_Grow")
    {
        TArray<int32> Array{1, 2, 3, 4, 5};
        Array.Resize(1000);
        return Array.Num();
    };

    BENCHMARK("Resize_Shrink")
    {
        TArray<int32> Array;
        Array.Resize(1000);
        Array.Resize(10);
        return Array.Num();
    };

    BENCHMARK("Clear")
    {
        TArray<int32> Array;
        Array.Resize(1000);
        Array.Clear();
        return Array.Num();
    };

    BENCHMARK("ShrinkToFit")
    {
        TArray<int32> Array;
        Array.Reserve(1000);
        for (int32 Index = 0; Index < 100; ++Index)
        {
            Array.PushBack(Index);
        }
        Array.ShrinkToFit();
        return Array.GetCapacity();
    };
}

TEST_CASE_METHOD(FMediumArrayFixture, "TArray::BenchmarkComparison", "[Array][.benchmark]")
{
    TArray<int32> Array2;
    Array2.Resize(100);
    for (int32 Index = 0; Index < 100; ++Index)
    {
        Array2[Index] = 42;
    }

    TArray<int32> Array3;
    Array3.Resize(100);
    for (int32 Index = 0; Index < 100; ++Index)
    {
        Array3[Index] = 43;
    }

    BENCHMARK("EqualityTrue")
    {
        return MediumArray == Array2;
    };

    BENCHMARK("EqualityFalse")
    {
        return MediumArray == Array3;
    };

    BENCHMARK("ThreeWayComparison")
    {
        return MediumArray <=> Array2;
    };
}

TEST_CASE("TArray::BenchmarkComplexType", "[Array][.benchmark]")
{
    BENCHMARK("StructPushBack")
    {
        TArray<FTestStruct> Array;
        Array.Reserve(100);
        for (int32 Index = 0; Index < 100; ++Index)
        {
            Array.PushBack(FTestStruct{.Value = Index, .Score = static_cast<float32>(Index)});
        }
        return Array.Num();
    };

    BENCHMARK("StructIteration")
    {
        TArray<FTestStruct> Array;
        Array.Resize(100);
        int32 Sum = 0;
        for (const FTestStruct& Element : Array)
        {
            Sum += Element.Value;
        }
        return Sum;
    };

    BENCHMARK("StructCopy")
    {
        TArray<FTestStruct> Original;
        Original.Resize(100);
        TArray<FTestStruct> Copy = Original;
        return Copy.Num();
    };
}
