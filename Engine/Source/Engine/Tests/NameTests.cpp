#include <catch2/catch_test_macros.hpp>
#include "Engine/Core/Containers/Name.hpp"

TEST_CASE("FNameStringPool", "[FNameStringPool]")
{
    SECTION("IsRegistered returns false for unregistered hash")
    {
        uint64 TestHash = 12345;
        REQUIRE(FNameStringPool::IsRegistered(TestHash) == false);
    }

    SECTION("Register adds string to pool")
    {
        uint64 TestHash = 67890;
        FString TestString("TestString");

        FNameStringPool::Register(TestHash, TestString);

        REQUIRE(FNameStringPool::IsRegistered(TestHash) == true);
    }

    SECTION("Register does not overwrite existing hash")
    {
        uint64 TestHash = 11111;
        FString FirstString("First");
        FString SecondString("Second");

        FNameStringPool::Register(TestHash, FirstString);
        FNameStringPool::Register(TestHash, SecondString);

        REQUIRE(FNameStringPool::GetString(TestHash) == FirstString);
    }

    SECTION("GetString returns empty string for unregistered hash")
    {
        uint64 UnregisteredHash = 99999;

        REQUIRE(FNameStringPool::GetString(UnregisteredHash).empty() == true);
    }

    SECTION("GetString returns registered string")
    {
        uint64 TestHash = 22222;
        FString TestString("RegisteredString");

        FNameStringPool::Register(TestHash, TestString);

        REQUIRE(FNameStringPool::GetString(TestHash) == TestString);
    }

    SECTION("GetStringView returns view of registered string")
    {
        uint64 TestHash = 33333;
        FString TestString("ViewString");

        FNameStringPool::Register(TestHash, TestString);
        FStringView View = FNameStringPool::GetStringView(TestHash);

        REQUIRE(View == FStringView(TestString));
    }
}

TEST_CASE("FName", "[FName]")
{
    SECTION("Default constructor creates invalid FName")
    {
        FName Name;

        REQUIRE(Name.IsValid() == false);
        REQUIRE(Name.GetHash() == 0);
    }

    SECTION("Constructor with string view creates valid FName")
    {
        FStringView TestView("TestName");
        FName Name(TestView);

        REQUIRE(Name.IsValid() == true);
        REQUIRE(Name.GetHash() != 0);
    }

    SECTION("GetString returns correct string")
    {
        FStringView TestView("MyTestString");
        FName Name(TestView);

        FString NameString = Name.GetString();
        REQUIRE(NameString == FString("MyTestString"));
    }

    SECTION("GetStringView returns correct string view")
    {
        FStringView TestView("ViewTest");
        FName Name(TestView);

        FStringView ResultView = Name.GetStringView();
        REQUIRE(ResultView == TestView);
    }

    SECTION("Multiple FNames with same string have same hash")
    {
        FStringView TestView("SameName");
        FName Name1(TestView);
        FName Name2(TestView);

        REQUIRE(Name1.GetHash() == Name2.GetHash());
    }

    SECTION("FNames with different strings have different hashes")
    {
        FName Name1(FStringView("FirstName"));
        FName Name2(FStringView("SecondName"));

        REQUIRE(Name1.GetHash() != Name2.GetHash());
    }

    SECTION("Copy constructor works correctly")
    {
        FName Original(FStringView("Original"));
        FName Copy(Original);

        REQUIRE(Copy.GetHash() == Original.GetHash());
        REQUIRE(Copy.GetString() == Original.GetString());
    }

    SECTION("Move constructor works correctly")
    {
        FName Original(FStringView("Moveable"));
        uint64 OriginalHash = Original.GetHash();

        FName Moved(std::move(Original));

        REQUIRE(Moved.GetHash() == OriginalHash);
        REQUIRE(Moved.GetString() == FString("Moveable"));
    }

    SECTION("Copy assignment works correctly")
    {
        FName Original(FStringView("ToCopy"));
        FName Target;

        Target = Original;

        REQUIRE(Target.GetHash() == Original.GetHash());
        REQUIRE(Target.GetString() == Original.GetString());
    }

    SECTION("Move assignment works correctly")
    {
        FName Original(FStringView("ToMove"));
        uint64 OriginalHash = Original.GetHash();
        FName Target;

        Target = std::move(Original);

        REQUIRE(Target.GetHash() == OriginalHash);
        REQUIRE(Target.GetString() == FString("ToMove"));
    }

    SECTION("uint64 cast operator returns hash")
    {
        FName Name(FStringView("CastTest"));
        uint64 Hash = Name.GetHash();

        uint64 CastedValue = Name;
        REQUIRE(CastedValue == Hash);
    }

    SECTION("const uint64 cast operator returns hash")
    {
        const FName Name(FStringView("ConstCastTest"));
        uint64 Hash = Name.GetHash();

        const uint64 CastedValue = Name;
        REQUIRE(CastedValue == Hash);
    }

    SECTION("Empty string creates valid FName with non-zero hash")
    {
        FName Name(FStringView(""));

        REQUIRE(Name.IsValid() == true);
        REQUIRE(Name.GetHash() != 0);
        REQUIRE(Name.GetString().empty() == true);
    }
}
