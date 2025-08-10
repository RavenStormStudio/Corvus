#include <numbers>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_contains.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include "Engine/Core/Containers/String.hpp"

TEST_CASE("String::Format with std::string", "String:Format")
{
    SECTION("Basic string formatting")
    {
        FString Result = String::Format("Hello, {}!", "World");
        REQUIRE(Result == "Hello, World!");
    }

    SECTION("Multiple arguments")
    {
        FString Result = String::Format("{} + {} = {}", 2, 3, 5);
        REQUIRE(Result == "2 + 3 = 5");
    }

    SECTION("Named placeholders with numbers")
    {
        FString Result = String::Format("Player {0} scored {1} points", "Alice", 100);
        REQUIRE(Result == "Player Alice scored 100 points");
    }

    SECTION("Different data types")
    {
        FString Result = String::Format("Float: {:.2f}, Int: {}, Bool: {}", std::numbers::pi_v<float32>, 42, true);
        REQUIRE(Result == "Float: 3.14, Int: 42, Bool: true");
    }

    SECTION("Empty format string")
    {
        FString Result = String::Format("");
        REQUIRE(Result.empty());
    }

    SECTION("Format with no arguments")
    {
        FString Result = String::Format("No placeholders here");
        REQUIRE(Result == "No placeholders here");
    }

    SECTION("Special characters")
    {
        FString Result = String::Format("Special: {}", "!@#$%^&*()");
        REQUIRE(Result == "Special: !@#$%^&*()");
    }
}

TEST_CASE("String::Format with std::wstring", "String:Format:Wide")
{
    SECTION("Basic wide string formatting")
    {
        FWideString Result = String::Format(L"Hello, {}!", L"World");
        REQUIRE(Result == L"Hello, World!");
    }

    SECTION("Multiple wide arguments")
    {
        FWideString Result = String::Format(L"{} + {} = {}", 10, 20, 30);
        REQUIRE(Result == L"10 + 20 = 30");
    }

    SECTION("Wide string with numbers")
    {
        FWideString Result = String::Format(L"Value: {}", 123);
        REQUIRE(Result == L"Value: 123");
    }

    SECTION("Empty wide format string")
    {
        FWideString Result = String::Format(L"");
        REQUIRE(Result.empty());
    }
}

TEST_CASE("String::Convert from FString to FWideString", "String:Convert")
{
    SECTION("Basic ASCII conversion")
    {
        const FString Input = "Hello, World!";
        const FWideString Result = String::Convert(Input);
        REQUIRE(Result == L"Hello, World!");
    }

    SECTION("Empty string conversion")
    {
        const FString Input;
        const FWideString Result = String::Convert(Input);
        REQUIRE(Result.empty());
    }

    SECTION("Single character")
    {
        const FString Input = "A";
        const FWideString Result = String::Convert(Input);
        REQUIRE(Result == L"A");
    }

    SECTION("Numbers and symbols")
    {
        const FString Input = "123!@#$%^&*()";
        const FWideString Result = String::Convert(Input);
        REQUIRE(Result == L"123!@#$%^&*()");
    }

    SECTION("Spaces and special characters")
    {
        const FString Input = "  Tab\t New\nLine\r Return  ";
        const FWideString Result = String::Convert(Input);
        REQUIRE(Result == L"  Tab\t New\nLine\r Return  ");
    }

    SECTION("Long string")
    {
        const FString Input(1000, 'A');
        const FWideString Result = String::Convert(Input);
        REQUIRE(Result.size() == 1000);
        REQUIRE(Result == FWideString(1000, L'A'));
    }
}

TEST_CASE("String::Convert from FWideString to FString", "String:Convert")
{
    SECTION("Basic ASCII conversion")
    {
        const FWideString Input = L"Hello, World!";
        const FString Result = String::Convert(Input);
        REQUIRE(Result == "Hello, World!");
    }

    SECTION("Empty wide string conversion")
    {
        const FWideString Input;
        const FString Result = String::Convert(Input);
        REQUIRE(Result.empty());
    }

    SECTION("Single wide character")
    {
        const FWideString Input = L"Z";
        const FString Result = String::Convert(Input);
        REQUIRE(Result == "Z");
    }

    SECTION("Numbers and symbols")
    {
        const FWideString Input = L"789!@#$%^&*()";
        const FString Result = String::Convert(Input);
        REQUIRE(Result == "789!@#$%^&*()");
    }

    SECTION("Whitespace characters")
    {
        const FWideString Input = L"  Space\t Tab\n Newline\r Return  ";
        const FString Result = String::Convert(Input);
        REQUIRE(Result == "  Space\t Tab\n Newline\r Return  ");
    }

    SECTION("Long wide string")
    {
        const FWideString Input(500, L'B');
        const FString Result = String::Convert(Input);
        REQUIRE(Result.size() == 500);
        REQUIRE(Result == FString(500, 'B'));
    }
}

TEST_CASE("String::Convert Round-trip Conversion", "String:Convert:Roundtrip")
{
    SECTION("ASCII round-trip")
    {
        const FString Original = "Round-trip test!";
        const FWideString Wide = String::Convert(Original);
        const FString ConvertedBack = String::Convert(Wide);
        REQUIRE(ConvertedBack == Original);
    }

    SECTION("Empty string round-trip")
    {
        const FString Original;
        const FWideString Wide = String::Convert(Original);
        const FString ConvertedBack = String::Convert(Wide);
        REQUIRE(ConvertedBack == Original);
        REQUIRE(ConvertedBack.empty());
    }

    SECTION("Special characters round-trip")
    {
        const FString Original = "!@#$%^&*()_+-=[]{}|;:'\",.<>?/~`";
        const FWideString Wide = String::Convert(Original);
        const FString ConvertedBack = String::Convert(Wide);
        REQUIRE(ConvertedBack == Original);
    }

    SECTION("Numbers round-trip")
    {
        const FString Original = "0123456789";
        const FWideString Wide = String::Convert(Original);
        const FString ConvertedBack = String::Convert(Wide);
        REQUIRE(ConvertedBack == Original);
    }

    SECTION("Whitespace round-trip")
    {
        const FString Original = " \t\n\r ";
        const FWideString Wide = String::Convert(Original);
        const FString ConvertedBack = String::Convert(Wide);
        REQUIRE(ConvertedBack == Original);
    }
}

TEST_CASE("String::Convert Wide Round-trip Conversion", "String:Convert:Roundtrip:Wide")
{
    SECTION("Wide ASCII round-trip")
    {
        const FWideString Original = L"Wide round-trip test!";
        const FString Narrow = String::Convert(Original);
        const FWideString ConvertedBack = String::Convert(Narrow);
        REQUIRE(ConvertedBack == Original);
    }

    SECTION("Wide empty string round-trip")
    {
        const FWideString Original;
        const FString Narrow = String::Convert(Original);
        const FWideString ConvertedBack = String::Convert(Narrow);
        REQUIRE(ConvertedBack == Original);
        REQUIRE(ConvertedBack.empty());
    }

    SECTION("Wide special characters round-trip")
    {
        const FWideString Original = L"!@#$%^&*()_+-=[]{}|;:'\",.<>?/~`";
        const FString Narrow = String::Convert(Original);
        const FWideString ConvertedBack = String::Convert(Narrow);
        REQUIRE(ConvertedBack == Original);
    }
}

TEST_CASE("String Integration Tests", "String:Integration")
{
    SECTION("Format then convert")
    {
        const FString Formatted = String::Format("Player {} scored {} points", "Alice", 150);
        const FWideString WideResult = String::Convert(Formatted);
        REQUIRE(WideResult == L"Player Alice scored 150 points");
    }

    SECTION("Wide format then convert")
    {
        const FWideString WideFormatted = String::Format(L"Level {}: {}", 5, L"Boss Fight");
        const FString NarrowResult = String::Convert(WideFormatted);
        REQUIRE(NarrowResult == "Level 5: Boss Fight");
    }

    SECTION("Convert then format")
    {
        const FString Original = "Corvus Engine";
        const FWideString Wide = String::Convert(Original);
        const FWideString Formatted = String::Format(L"Welcome to {}!", Wide);
        REQUIRE(Formatted == L"Welcome to Corvus Engine!");
    }

    SECTION("Complex formatting with conversions")
    {
        const FString EngineName = "Corvus";
        const FWideString WideEngine = String::Convert(EngineName);

        const FWideString StatusMessage = String::Format(L"{} Engine: {} FPS", WideEngine, 60);
        const FString FinalMessage = String::Convert(StatusMessage);

        REQUIRE(FinalMessage == "Corvus Engine: 60 FPS");
    }
}

TEST_CASE("String Edge Cases", "String:Edge")
{
    SECTION("Very long strings")
    {
        const FString LongString(10000, 'X');
        const FWideString WideLong = String::Convert(LongString);
        REQUIRE(WideLong.size() == 10000);

        const FString ConvertedBack = String::Convert(WideLong);
        REQUIRE(ConvertedBack == LongString);
    }

    SECTION("String with null characters (should be handled carefully)")
    {
        FString WithNull = "Before";
        WithNull.push_back('\0');
        WithNull += "After";

        const FWideString WideResult = String::Convert(WithNull);
        const FString ConvertedBack = String::Convert(WideResult);

        REQUIRE_FALSE(ConvertedBack.empty());
    }
}
