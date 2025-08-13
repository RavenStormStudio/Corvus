#include <catch2/catch_test_macros.hpp>

#include "Engine/Core/Utility/Guid.hpp"

TEST_CASE("FGuid", "[FGuid]")
{
    SECTION("Default constructor creates invalid FGuid")
    {
        FGuid TestGuid;
        REQUIRE(TestGuid.IsValid() == false);
    }

    SECTION("Parameterized constructor creates valid FGuid")
    {
        FGuid TestGuid(0x12345678, 0x9ABC, 0xDEF0, 0x12345678);
        REQUIRE(TestGuid.IsValid() == true);
    }

    SECTION("All zero parameters create invalid FGuid")
    {
        FGuid TestGuid(0, 0, 0, 0);
        REQUIRE(TestGuid.IsValid() == false);
    }

    SECTION("String constructor with valid string creates valid FGuid")
    {
        FString ValidGuidString = "12345678-9abc-def0-1234-567890abcdef";
        FGuid TestGuid(ValidGuidString);
        REQUIRE(TestGuid.IsValid() == true);
    }

    SECTION("String constructor with invalid string creates invalid FGuid")
    {
        FString InvalidGuidString = "invalid-guid-string";
        FGuid TestGuid(InvalidGuidString);
        REQUIRE(TestGuid.IsValid() == false);
    }

    SECTION("Invalidate makes FGuid invalid")
    {
        FGuid TestGuid(0x12345678, 0x9ABC, 0xDEF0, 0x12345678);
        TestGuid.Invalidate();
        REQUIRE(TestGuid.IsValid() == false);
    }

    SECTION("ToString produces correct format")
    {
        FGuid TestGuid(0x12345678, 0x9ABCDEF0, 0x12345678, 0x9ABCDEF0);
        FString Result = TestGuid.ToString();
        REQUIRE(Result == "12345678-9abc-def0-1234-56789abcdef0");
    }

    SECTION("Equality operator returns true for identical FGuids")
    {
        FGuid FirstGuid(0x12345678, 0x9ABC, 0xDEF0, 0x12345678);
        FGuid SecondGuid(0x12345678, 0x9ABC, 0xDEF0, 0x12345678);
        REQUIRE((FirstGuid == SecondGuid) == true);
    }

    SECTION("Equality operator returns false for different FGuids")
    {
        FGuid FirstGuid(0x12345678, 0x9ABC, 0xDEF0, 0x12345678);
        FGuid SecondGuid(0x87654321, 0xCBA9, 0x0FED, 0x87654321);
        REQUIRE((FirstGuid == SecondGuid) == false);
    }

    SECTION("Inequality operator returns true for different FGuids")
    {
        FGuid FirstGuid(0x12345678, 0x9ABC, 0xDEF0, 0x12345678);
        FGuid SecondGuid(0x87654321, 0xCBA9, 0x0FED, 0x87654321);
        REQUIRE((FirstGuid != SecondGuid) == true);
    }

    SECTION("Less than operator returns true when first FGuid is smaller")
    {
        FGuid FirstGuid(0x00000001, 0x0000, 0x0000, 0x00000000);
        FGuid SecondGuid(0x00000002, 0x0000, 0x0000, 0x00000000);
        REQUIRE((FirstGuid < SecondGuid) == true);
    }

    SECTION("Greater than operator returns true when first FGuid is larger")
    {
        FGuid FirstGuid(0x00000002, 0x0000, 0x0000, 0x00000000);
        FGuid SecondGuid(0x00000001, 0x0000, 0x0000, 0x00000000);
        REQUIRE((FirstGuid > SecondGuid) == true);
    }

    SECTION("NewGuid generates valid FGuid")
    {
        FGuid NewGuid = FGuid::NewGuid();
        REQUIRE(NewGuid.IsValid() == true);
    }

    SECTION("NewGuid generates unique FGuids")
    {
        FGuid FirstGuid = FGuid::NewGuid();
        FGuid SecondGuid = FGuid::NewGuid();
        REQUIRE((FirstGuid != SecondGuid) == true);
    }

    SECTION("Parse with C string and valid format returns true")
    {
        FGuid OutGuid;
        bool8 bResult = FGuid::Parse("12345678-9abc-def0-1234-567890abcdef", OutGuid);
        REQUIRE(bResult == true);
        REQUIRE(OutGuid.IsValid() == true);
    }

    SECTION("Parse with C string and invalid format returns false")
    {
        FGuid OutGuid;
        bool8 bResult = FGuid::Parse("invalid-format", OutGuid);
        REQUIRE(bResult == false);
    }

    SECTION("Parse with FString and valid format returns true")
    {
        FGuid OutGuid;
        FString TestString = "12345678-9abc-def0-1234-567890abcdef";
        bool8 bResult = FGuid::Parse(TestString, OutGuid);
        REQUIRE(bResult == true);
        REQUIRE(OutGuid.IsValid() == true);
    }

    SECTION("Parse with FStringView and valid format returns true")
    {
        FGuid OutGuid;
        FStringView TestStringView = "12345678-9abc-def0-1234-567890abcdef";
        bool8 bResult = FGuid::Parse(TestStringView, OutGuid);
        REQUIRE(bResult == true);
        REQUIRE(OutGuid.IsValid() == true);
    }

    SECTION("Parse with wrong length returns false")
    {
        FGuid OutGuid;
        FStringView TestStringView = "12345678-9abc-def0";
        bool8 bResult = FGuid::Parse(TestStringView, OutGuid);
        REQUIRE(bResult == false);
    }

    SECTION("Parse with missing dashes returns false")
    {
        FGuid OutGuid;
        FStringView TestStringView = "123456789abcdef01234567890abcdef01234";
        bool8 bResult = FGuid::Parse(TestStringView, OutGuid);
        REQUIRE(bResult == false);
    }

    SECTION("Parse and ToString round trip preserves value")
    {
        FString OriginalString = "12345678-9abc-def0-1234-567890abcdef";
        FGuid ParsedGuid;
        FGuid::Parse(OriginalString, ParsedGuid);
        FString ResultString = ParsedGuid.ToString();
        REQUIRE(ResultString == OriginalString);
    }
}
