// RavenStorm Copyright @ 2025-2025

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include "Core/Containers/StaticArray.hpp"

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

TEST_CASE("TStaticArray::DefaultConstruction", "[StaticArray]")
{
    TStaticArray<int32, 5> Array;

    REQUIRE(Array.Num() == 5);
    REQUIRE(Array.GetSizeInBytes() == 5 * sizeof(int32));
}

TEST_CASE("TStaticArray::ConstructionWithInitialValue", "[StaticArray]")
{
    TStaticArray<int32, 4> Array(42);

    REQUIRE(Array[0] == 42);
    REQUIRE(Array[1] == 42);
    REQUIRE(Array[2] == 42);
    REQUIRE(Array[3] == 42);
}

TEST_CASE("TStaticArray::ConstructionWithInitializerList", "[StaticArray]")
{
    SECTION("FullInitializerList")
    {
        TStaticArray<int32, 5> Array{1, 2, 3, 4, 5};

        REQUIRE(Array[0] == 1);
        REQUIRE(Array[1] == 2);
        REQUIRE(Array[2] == 3);
        REQUIRE(Array[3] == 4);
        REQUIRE(Array[4] == 5);
    }

    SECTION("PartialInitializerList")
    {
        TStaticArray<int32, 5> Array{10, 20, 30};

        REQUIRE(Array[0] == 10);
        REQUIRE(Array[1] == 20);
        REQUIRE(Array[2] == 30);
    }
}

TEST_CASE("TStaticArray::CopyConstruction", "[StaticArray]")
{
    TStaticArray<int32, 3> Original{100, 200, 300};
    TStaticArray<int32, 3> Copy(Original);

    REQUIRE(Copy[0] == 100);
    REQUIRE(Copy[1] == 200);
    REQUIRE(Copy[2] == 300);

    Copy[0] = 999;
    REQUIRE(Original[0] == 100);
}

TEST_CASE("TStaticArray::MoveConstruction", "[StaticArray]")
{
    TStaticArray<int32, 3> Original{100, 200, 300};
    TStaticArray<int32, 3> Moved(std::move(Original));

    REQUIRE(Moved[0] == 100);
    REQUIRE(Moved[1] == 200);
    REQUIRE(Moved[2] == 300);
}

TEST_CASE("TStaticArray::CopyAssignment", "[StaticArray]")
{
    TStaticArray<int32, 3> Source{10, 20, 30};
    TStaticArray<int32, 3> Destination = Source;

    REQUIRE(Destination[0] == 10);
    REQUIRE(Destination[1] == 20);
    REQUIRE(Destination[2] == 30);

    Destination[0] = 999;
    REQUIRE(Source[0] == 10);
}

TEST_CASE("TStaticArray::MoveAssignment", "[StaticArray]")
{
    TStaticArray<int32, 3> Source{10, 20, 30};
    TStaticArray<int32, 3> Destination = std::move(Source);

    REQUIRE(Destination[0] == 10);
    REQUIRE(Destination[1] == 20);
    REQUIRE(Destination[2] == 30);
}

TEST_CASE("TStaticArray::SelfAssignment", "[StaticArray]")
{
    TStaticArray<int32, 3> Array{1, 2, 3};

    Array = Array;

    REQUIRE(Array[0] == 1);
    REQUIRE(Array[1] == 2);
    REQUIRE(Array[2] == 3);
}

TEST_CASE("TStaticArray::ElementAccess", "[StaticArray]")
{
    TStaticArray<int32, 5> Array{1, 2, 3, 4, 5};

    SECTION("OperatorBrackets")
    {
        REQUIRE(Array[0] == 1);
        REQUIRE(Array[3] == 4);

        Array[1] = 99;
        REQUIRE(Array[1] == 99);
    }

    SECTION("ConstOperatorBrackets")
    {
        const TStaticArray<int32, 5>& ConstArray = Array;
        REQUIRE(ConstArray[0] == 1);
        REQUIRE(ConstArray[3] == 4);
    }

    SECTION("AtMethod")
    {
        REQUIRE(Array.At(0) == 1);
        REQUIRE(Array.At(3) == 4);

        Array.At(2) = 88;
        REQUIRE(Array.At(2) == 88);
    }

    SECTION("ConstAtMethod")
    {
        const TStaticArray<int32, 5>& ConstArray = Array;
        REQUIRE(ConstArray.At(0) == 1);
        REQUIRE(ConstArray.At(3) == 4);
    }
}

TEST_CASE("TStaticArray::GetFirstAndGetLast", "[StaticArray]")
{
    TStaticArray<int32, 5> Array{1, 2, 3, 4, 5};

    SECTION("GetFirst")
    {
        REQUIRE(Array.GetFirst() == 1);

        Array.GetFirst() = 100;
        REQUIRE(Array.GetFirst() == 100);
    }

    SECTION("GetLast")
    {
        REQUIRE(Array.GetLast() == 5);

        Array.GetLast() = 500;
        REQUIRE(Array.GetLast() == 500);
    }

    SECTION("ConstGetFirst")
    {
        const TStaticArray<int32, 5>& ConstArray = Array;
        REQUIRE(ConstArray.GetFirst() == 1);
    }

    SECTION("ConstGetLast")
    {
        const TStaticArray<int32, 5>& ConstArray = Array;
        REQUIRE(ConstArray.GetLast() == 5);
    }
}

TEST_CASE("TStaticArray::GetData", "[StaticArray]")
{
    TStaticArray<int32, 3> Array{10, 20, 30};

    SECTION("NonConstGetData")
    {
        int32* DataPtr = Array.GetData();
        REQUIRE(DataPtr != nullptr);
        REQUIRE(DataPtr[0] == 10);
        REQUIRE(DataPtr[1] == 20);
        REQUIRE(DataPtr[2] == 30);

        DataPtr[1] = 99;
        REQUIRE(Array[1] == 99);
    }

    SECTION("ConstGetData")
    {
        const TStaticArray<int32, 3>& ConstArray = Array;
        const int32* DataPtr = ConstArray.GetData();
        REQUIRE(DataPtr != nullptr);
        REQUIRE(DataPtr[0] == 10);
        REQUIRE(DataPtr[2] == 30);
    }
}

TEST_CASE("TStaticArray::Fill", "[StaticArray]")
{
    TStaticArray<int32, 5> Array{1, 2, 3, 4, 5};

    Array.Fill(42);

    REQUIRE(Array[0] == 42);
    REQUIRE(Array[1] == 42);
    REQUIRE(Array[2] == 42);
    REQUIRE(Array[3] == 42);
    REQUIRE(Array[4] == 42);
}

TEST_CASE("TStaticArray::IteratorSupport", "[StaticArray]")
{
    TStaticArray<int32, 4> Array{10, 20, 30, 40};

    SECTION("BeginEnd")
    {
        int32* Begin = Array.begin();
        int32* End = Array.end();

        REQUIRE(End - Begin == 4);
        REQUIRE(*Begin == 10);
        REQUIRE(*(End - 1) == 40);
    }

    SECTION("ConstBeginEnd")
    {
        const TStaticArray<int32, 4>& ConstArray = Array;
        const int32* Begin = ConstArray.begin();
        const int32* End = ConstArray.end();

        REQUIRE(End - Begin == 4);
        REQUIRE(*Begin == 10);
    }

    SECTION("CBeginCEnd")
    {
        const int32* Begin = Array.cbegin();
        const int32* End = Array.cend();

        REQUIRE(End - Begin == 4);
        REQUIRE(*Begin == 10);
    }

    SECTION("RangeBasedForLoop")
    {
        int32 Sum = 0;
        for (const int32& Element : Array)
        {
            Sum += Element;
        }
        REQUIRE(Sum == 100);
    }

    SECTION("RangeBasedForLoopModification")
    {
        for (int32& Element : Array)
        {
            Element *= 2;
        }

        REQUIRE(Array[0] == 20);
        REQUIRE(Array[1] == 40);
        REQUIRE(Array[2] == 60);
        REQUIRE(Array[3] == 80);
    }
}

TEST_CASE("TStaticArray::EqualityComparison", "[StaticArray]")
{
    TStaticArray<int32, 3> Array1{1, 2, 3};
    TStaticArray<int32, 3> Array2{1, 2, 3};
    TStaticArray<int32, 3> Array3{1, 2, 4};

    SECTION("Equality")
    {
        REQUIRE(Array1 == Array2);
        REQUIRE_FALSE(Array1 == Array3);
    }
}

TEST_CASE("TStaticArray::ThreeWayComparison", "[StaticArray]")
{
    const TStaticArray<int32, 3> Array1{1, 2, 3};
    const TStaticArray<int32, 3> Array2{1, 2, 3};
    const TStaticArray<int32, 3> Array3{1, 2, 4};
    const TStaticArray<int32, 3> Array4{1, 2, 2};

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
}

TEST_CASE("TStaticArray::StaticMethods", "[StaticArray]")
{
    SECTION("Num")
    {
        REQUIRE(TStaticArray<int32, 10>::Num() == 10);
        REQUIRE(TStaticArray<float32, 7>::Num() == 7);
    }

    SECTION("GetSizeInBytes")
    {
        REQUIRE(TStaticArray<int32, 5>::GetSizeInBytes() == 5 * sizeof(int32));
        REQUIRE(TStaticArray<float64, 3>::GetSizeInBytes() == 3 * sizeof(float64));
    }
}

TEST_CASE("TStaticArray::DeductionGuide", "[StaticArray]")
{
    TStaticArray Array{1, 2, 3, 4, 5};

    REQUIRE(Array.Num() == 5);
    REQUIRE(Array[0] == 1);
    REQUIRE(Array[4] == 5);
}

TEST_CASE("TStaticArray::FloatTypes", "[StaticArray]")
{
    TStaticArray<float32, 3> FloatArray{1.5f, 2.5f, 3.5f};

    REQUIRE(FloatArray[0] == 1.5f);
    REQUIRE(FloatArray[1] == 2.5f);
    REQUIRE(FloatArray[2] == 3.5f);

    FloatArray.Fill(9.9f);
    REQUIRE(FloatArray[0] == 9.9f);
}

TEST_CASE("TStaticArray::CustomStructType", "[StaticArray]")
{
    SECTION("Construction")
    {
        TStaticArray<FTestStruct, 3> StructArray{
            FTestStruct{.Value = 1, .Score = 1.0f},
            FTestStruct{.Value = 2, .Score = 2.0f},
            FTestStruct{.Value = 3, .Score = 3.0f}
        };

        REQUIRE(StructArray[0].Value == 1);
        REQUIRE(StructArray[0].Score == 1.0f);
        REQUIRE(StructArray[2].Value == 3);
    }

    SECTION("Comparison")
    {
        TStaticArray<FTestStruct, 2> Array1{FTestStruct{.Value = 1, .Score = 1.0f}, FTestStruct{.Value = 2, .Score = 2.0f}};
        TStaticArray<FTestStruct, 2> Array2{FTestStruct{.Value = 1, .Score = 1.0f}, FTestStruct{.Value = 2, .Score = 2.0f}};

        REQUIRE(Array1 == Array2);
    }
}

TEST_CASE("TStaticArray::ConstexprSupport", "[StaticArray]")
{
    constexpr TStaticArray<int32, 3> ConstexprArray{10, 20, 30};

    static_assert(ConstexprArray[0] == 10);
    static_assert(ConstexprArray[1] == 20);
    static_assert(ConstexprArray[2] == 30);
    static_assert(TStaticArray<int, 3>::Num() == 3);
    static_assert(ConstexprArray.GetFirst() == 10);
    static_assert(ConstexprArray.GetLast() == 30);
}

TEST_CASE("TStaticArray::SingleElementArray", "[StaticArray]")
{
    TStaticArray<int32, 1> SingleArray{42};

    REQUIRE(SingleArray.Num() == 1);
    REQUIRE(SingleArray[0] == 42);
    REQUIRE(SingleArray.GetFirst() == 42);
    REQUIRE(SingleArray.GetLast() == 42);
}

TEST_CASE("TStaticArray::LargeArray", "[StaticArray]")
{
    constexpr size64 LargeSize = 1000;
    TStaticArray<int32, LargeSize> LargeArray(42);

    REQUIRE(LargeArray.Num() == LargeSize);
    REQUIRE(LargeArray[0] == 42);
    REQUIRE(LargeArray[LargeSize - 1] == 42);
}

TEST_CASE("TStaticArray::BenchmarkConstruction", "[StaticArray][.benchmark]")
{
    BENCHMARK("DefaultConstruction")
    {
        return TStaticArray<int, 100>::Num();
    };

    BENCHMARK("InitialValueConstruction")
    {
        TStaticArray<int32, 100> Array(42);
        return Array[0];
    };

    BENCHMARK("InitializerListConstruction")
    {
        TStaticArray<int32, 5> Array{1, 2, 3, 4, 5};
        return Array[0];
    };

    BENCHMARK("CopyConstruction")
    {
        const TStaticArray<int32, 100> Original(42);
        TStaticArray<int32, 100> Copy(Original);
        return Copy[0];
    };

    BENCHMARK("MoveConstruction")
    {
        TStaticArray<int32, 100> Original(42);
        TStaticArray<int32, 100> Moved(std::move(Original));
        return Moved[0];
    };
}

TEST_CASE("TStaticArray::BenchmarkElementAccess", "[StaticArray][.benchmark]")
{
    TStaticArray<int32, 1000> Array(42);

    BENCHMARK("SequentialRead")
    {
        int32 Sum = 0;
        for (size64 Index = 0; Index < Array.Num(); ++Index)
        {
            Sum += Array[Index];
        }
        return Sum;
    };

    BENCHMARK("SequentialWrite")
    {
        for (size64 Index = 0; Index < Array.Num(); ++Index)
        {
            Array[Index] = static_cast<int32>(Index);
        }
        return Array[0];
    };

    BENCHMARK("RandomAccess")
    {
        int32 Sum = 0;
        Sum += Array[0];
        Sum += Array[500];
        Sum += Array[999];
        return Sum;
    };
}

TEST_CASE("TStaticArray::BenchmarkIteration", "[StaticArray][.benchmark]")
{
    TStaticArray<int32, 1000> Array(42);

    BENCHMARK("RangeBasedForLoopRead")
    {
        int32 Sum = 0;
        for (const int32& Element : Array)
        {
            Sum += Element;
        }
        return Sum;
    };

    BENCHMARK("RangeBasedForLoopWrite")
    {
        for (int32& Element : Array)
        {
            Element += 1;
        }
        return Array[0];
    };

    BENCHMARK("PointerIteration")
    {
        int32 Sum = 0;
        const int32* Begin = Array.begin();
        const int32* End = Array.end();
        for (const int32* Ptr = Begin; Ptr != End; ++Ptr)
        {
            Sum += *Ptr;
        }
        return Sum;
    };
}

TEST_CASE("TStaticArray::BenchmarkOperations", "[StaticArray][.benchmark]")
{
    BENCHMARK("Fill_SmallArray")
    {
        TStaticArray<int32, 10> Array;
        Array.Fill(42);
        return Array[0];
    };

    BENCHMARK("Fill_MediumArray")
    {
        TStaticArray<int32, 100> Array;
        Array.Fill(42);
        return Array[0];
    };

    BENCHMARK("Fill_LargeArray")
    {
        TStaticArray<int32, 1000> Array;
        Array.Fill(42);
        return Array[0];
    };

    BENCHMARK("CopyAssignment")
    {
        const TStaticArray<int32, 100> Source(42);
        TStaticArray<int32, 100> Destination = Source;
        return Destination[0];
    };

    BENCHMARK("MoveAssignment")
    {
        TStaticArray<int32, 100> Source(42);
        TStaticArray<int32, 100> Destination = std::move(Source);
        return Destination[0];
    };
}

TEST_CASE("TStaticArray::BenchmarkComparison", "[StaticArray][.benchmark]")
{
    const TStaticArray<int32, 100> Array1(42);
    const TStaticArray<int32, 100> Array2(42);
    const TStaticArray<int32, 100> Array3(43);

    BENCHMARK("EqualityTrue")
    {
        return Array1 == Array2;
    };

    BENCHMARK("EqualityFalse")
    {
        return Array1 == Array3;
    };

    BENCHMARK("ThreeWayComparison")
    {
        return Array1 <=> Array2;
    };
}

TEST_CASE("TStaticArray::BenchmarkComplexType", "[StaticArray][.benchmark]")
{
    BENCHMARK("StructConstruction")
    {
        return TStaticArray<FTestStruct, 100>::Num();
    };

    BENCHMARK("StructFill")
    {
        TStaticArray<FTestStruct, 100> Array;
        Array.Fill(FTestStruct{.Value = 42, .Score = 3.14f});
        return Array[0].Value;
    };

    BENCHMARK("StructIteration")
    {
        TStaticArray<FTestStruct, 100> Array;
        int32 Sum = 0;
        for (const FTestStruct& Element : Array)
        {
            Sum += Element.Value;
        }
        return Sum;
    };
}
