#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <thread>
#include "Engine/Core/Utility/Time.hpp"

using namespace Catch;

TEST_CASE("TDuration", "[TDuration]")
{
    SECTION("Default constructor creates zero duration")
    {
        FSeconds TestDuration;
        REQUIRE(TestDuration.GetCount() == 0.0);
        REQUIRE(TestDuration.IsZero() == true);
    }

    SECTION("Value constructor creates duration with specified value")
    {
        FSeconds TestDuration(2.5);
        REQUIRE(TestDuration.GetCount() == 2.5);
        REQUIRE(TestDuration.IsZero() == false);
        REQUIRE(TestDuration.IsPositive() == true);
        REQUIRE(TestDuration.IsNegative() == false);
    }

    SECTION("Negative value constructor creates negative duration")
    {
        FSeconds TestDuration(-3.5);
        REQUIRE(TestDuration.GetCount() == -3.5);
        REQUIRE(TestDuration.IsZero() == false);
        REQUIRE(TestDuration.IsPositive() == false);
        REQUIRE(TestDuration.IsNegative() == true);
    }

    SECTION("Zero value creates zero duration")
    {
        FSeconds TestDuration(0.0);
        REQUIRE(TestDuration.IsZero() == true);
        REQUIRE(TestDuration.IsPositive() == false);
        REQUIRE(TestDuration.IsNegative() == false);
    }

    SECTION("Copy constructor creates identical duration")
    {
        FSeconds OriginalDuration(4.2);
        FSeconds CopiedDuration(OriginalDuration);
        REQUIRE(CopiedDuration.GetCount() == 4.2);
        REQUIRE(CopiedDuration == OriginalDuration);
    }

    SECTION("Conversion from milliseconds to seconds works correctly")
    {
        FMilliSeconds TestMs(2500);
        FSeconds TestSeconds(TestMs);
        REQUIRE(TestSeconds.GetCount() == Approx(2.5));
    }

    SECTION("Conversion from seconds to milliseconds works correctly")
    {
        FSeconds TestSeconds(1.75);
        FMilliSeconds TestMs(TestSeconds);
        REQUIRE(TestMs.GetCount() == 1750);
    }

    SECTION("Conversion from nanoseconds to microseconds works correctly")
    {
        FNanoSeconds TestNs(7500);
        FMicroSeconds TestUs(TestNs);
        REQUIRE(TestUs.GetCount() == 7);
    }

    SECTION("Conversion from minutes to seconds works correctly")
    {
        FMinutes TestMinutes(1.5);
        FSeconds TestSeconds(TestMinutes);
        REQUIRE(TestSeconds.GetCount() == Approx(90.0));
    }

    SECTION("Conversion from hours to minutes works correctly")
    {
        FHours TestHours(2.5);
        FMinutes TestMinutes(TestHours);
        REQUIRE(TestMinutes.GetCount() == Approx(150.0));
    }

    SECTION("Addition of same type durations returns correct result")
    {
        FSeconds FirstDuration(2.5);
        FSeconds SecondDuration(1.5);
        auto Result = FirstDuration + SecondDuration;
        REQUIRE(Result.GetCount() == Approx(4.0));
    }

    SECTION("Addition of different type durations returns correct result")
    {
        FSeconds TestSeconds(1.0);
        FMilliSeconds TestMs(500);
        auto Result = TestSeconds + TestMs;
        REQUIRE(Result.GetCount() == Approx(1.5));
    }

    SECTION("Subtraction of same type durations returns correct result")
    {
        FSeconds FirstDuration(5.0);
        FSeconds SecondDuration(2.0);
        auto Result = FirstDuration - SecondDuration;
        REQUIRE(Result.GetCount() == Approx(3.0));
    }

    SECTION("Subtraction of different type durations returns correct result")
    {
        FSeconds TestSeconds(2.0);
        FMilliSeconds TestMs(750);
        auto Result = TestSeconds - TestMs;
        REQUIRE(Result.GetCount() == Approx(1.25));
    }

    SECTION("Multiplication by integer scalar returns correct result")
    {
        FSeconds TestDuration(3.0);
        auto Result = TestDuration * 4;
        REQUIRE(Result.GetCount() == Approx(12.0));
    }

    SECTION("Multiplication by float scalar returns correct result")
    {
        FSeconds TestDuration(2.0);
        auto Result = TestDuration * 2.5;
        REQUIRE(Result.GetCount() == Approx(5.0));
    }

    SECTION("Division by integer scalar returns correct result")
    {
        FSeconds TestDuration(15.0);
        auto Result = TestDuration / 3;
        REQUIRE(Result.GetCount() == Approx(5.0));
    }

    SECTION("Division by float scalar returns correct result")
    {
        FSeconds TestDuration(10.0);
        auto Result = TestDuration / 2.5;
        REQUIRE(Result.GetCount() == Approx(4.0));
    }

    SECTION("Addition assignment operator modifies duration correctly")
    {
        FSeconds TestDuration(3.0);
        TestDuration += FSeconds(2.0);
        REQUIRE(TestDuration.GetCount() == Approx(5.0));
    }

    SECTION("Subtraction assignment operator modifies duration correctly")
    {
        FSeconds TestDuration(8.0);
        TestDuration -= FSeconds(3.0);
        REQUIRE(TestDuration.GetCount() == Approx(5.0));
    }

    SECTION("Multiplication assignment operator modifies duration correctly")
    {
        FSeconds TestDuration(4.0);
        TestDuration *= 1.5;
        REQUIRE(TestDuration.GetCount() == Approx(6.0));
    }

    SECTION("Division assignment operator modifies duration correctly")
    {
        FSeconds TestDuration(12.0);
        TestDuration /= 3.0;
        REQUIRE(TestDuration.GetCount() == Approx(4.0));
    }

    SECTION("Equality operator returns true for identical durations")
    {
        FSeconds FirstDuration(2.5);
        FSeconds SecondDuration(2.5);
        REQUIRE((FirstDuration == SecondDuration) == true);
    }

    SECTION("Equality operator returns false for different durations")
    {
        FSeconds FirstDuration(2.5);
        FSeconds SecondDuration(3.0);
        REQUIRE((FirstDuration == SecondDuration) == false);
    }

    SECTION("Equality operator works across different duration types")
    {
        FSeconds TestSeconds(1.0);
        FMilliSeconds TestMs(1000);
        REQUIRE((TestSeconds == TestMs) == true);
    }

    SECTION("Less than operator returns true for smaller duration")
    {
        FSeconds SmallerDuration(1.0);
        FSeconds LargerDuration(3.0);
        REQUIRE((SmallerDuration < LargerDuration) == true);
    }

    SECTION("Greater than operator returns true for larger duration")
    {
        FSeconds SmallerDuration(1.0);
        FSeconds LargerDuration(3.0);
        REQUIRE((LargerDuration > SmallerDuration) == true);
    }

    SECTION("Less than or equal operator works correctly")
    {
        FSeconds FirstDuration(2.0);
        FSeconds SecondDuration(2.0);
        FSeconds SmallerDuration(1.0);
        REQUIRE((FirstDuration <= SecondDuration) == true);
        REQUIRE((SmallerDuration <= FirstDuration) == true);
    }

    SECTION("Absolute of positive duration returns same value")
    {
        FSeconds TestDuration(5.0);
        auto Result = TestDuration.Absolute();
        REQUIRE(Result.GetCount() == Approx(5.0));
    }

    SECTION("Absolute of negative duration returns positive value")
    {
        FSeconds TestDuration(-3.0);
        auto Result = TestDuration.Absolute();
        REQUIRE(Result.GetCount() == Approx(3.0));
    }

    SECTION("Absolute of zero duration returns zero")
    {
        FSeconds TestDuration(0.0);
        auto Result = TestDuration.Absolute();
        REQUIRE(Result.GetCount() == Approx(0.0));
    }

    SECTION("Zero static method creates zero duration")
    {
        auto ZeroDuration = FSeconds::Zero();
        REQUIRE(ZeroDuration.IsZero() == true);
        REQUIRE(ZeroDuration.GetCount() == 0.0);
    }

    SECTION("Min static method creates minimum duration")
    {
        auto MinDuration = FNanoSeconds::Min();
        REQUIRE(MinDuration.GetCount() < 0);
    }

    SECTION("Max static method creates maximum duration")
    {
        auto MaxDuration = FNanoSeconds::Max();
        REQUIRE(MaxDuration.GetCount() > 0);
    }

    SECTION("Min duration is less than Max duration")
    {
        auto MinDuration = FNanoSeconds::Min();
        auto MaxDuration = FNanoSeconds::Max();
        REQUIRE(MaxDuration.GetCount() > MinDuration.GetCount());
    }

    SECTION("GetNativeDuration returns correct std::chrono::duration")
    {
        FSeconds TestDuration(3.5);
        auto NativeDuration = TestDuration.GetNativeDuration();
        REQUIRE(NativeDuration.count() == Approx(3.5));
    }

    SECTION("Very small nanosecond duration is handled correctly")
    {
        FNanoSeconds TestDuration(1);
        REQUIRE(TestDuration.GetCount() == 1);
        REQUIRE(TestDuration.IsPositive() == true);
    }

    SECTION("Very large hour duration is handled correctly")
    {
        FHours TestDuration(1000000.0);
        REQUIRE(TestDuration.GetCount() == Approx(1000000.0));
        REQUIRE(TestDuration.IsPositive() == true);
    }
}

TEST_CASE("FTimePoint", "[FTimePoint]")
{
    SECTION("Default constructor creates time point at epoch")
    {
        FTimePoint TestTimePoint;
        REQUIRE(TestTimePoint.GetTicks() == 0);
        REQUIRE(TestTimePoint.GetSeconds().IsZero() == true);
    }

    SECTION("Copy constructor creates identical time point")
    {
        auto OriginalTime = FTimePoint::Now();
        FTimePoint CopiedTime(OriginalTime);
        REQUIRE(CopiedTime == OriginalTime);
    }

    SECTION("FromDuration creates time point with correct duration")
    {
        FSeconds TestDuration(7.5);
        auto TestTimePoint = FTimePoint::FromDuration(TestDuration);
        REQUIRE(TestTimePoint.GetSeconds().GetCount() == Approx(7.5));
    }

    SECTION("GetSeconds returns correct duration in seconds")
    {
        FSeconds TestDuration(4.25);
        auto TestTimePoint = FTimePoint::FromDuration(TestDuration);
        REQUIRE(TestTimePoint.GetSeconds().GetCount() == Approx(4.25));
    }

    SECTION("GetMilliSeconds returns correct duration in milliseconds")
    {
        FSeconds TestDuration(2.5);
        auto TestTimePoint = FTimePoint::FromDuration(TestDuration);
        REQUIRE(TestTimePoint.GetMilliSeconds().GetCount() == 2500);
    }

    SECTION("GetMicroSeconds returns correct duration in microseconds")
    {
        FSeconds TestDuration(0.003);
        auto TestTimePoint = FTimePoint::FromDuration(TestDuration);
        REQUIRE(TestTimePoint.GetMicroSeconds().GetCount() == 3000);
    }

    SECTION("GetNanoSeconds returns correct duration in nanoseconds")
    {
        FMilliSeconds TestDuration(1);
        auto TestTimePoint = FTimePoint::FromDuration(TestDuration);
        REQUIRE(TestTimePoint.GetNanoSeconds().GetCount() == 1000000);
    }

    SECTION("GetMinutes returns correct duration in minutes")
    {
        FSeconds TestDuration(180.0);
        auto TestTimePoint = FTimePoint::FromDuration(TestDuration);
        REQUIRE(TestTimePoint.GetMinutes().GetCount() == Approx(3.0));
    }

    SECTION("GetHours returns correct duration in hours")
    {
        FSeconds TestDuration(7200.0);
        auto TestTimePoint = FTimePoint::FromDuration(TestDuration);
        REQUIRE(TestTimePoint.GetHours().GetCount() == Approx(2.0));
    }

    SECTION("Addition with duration creates later time point")
    {
        auto BaseTime = FTimePoint::FromDuration(FSeconds(10.0));
        auto LaterTime = BaseTime + FSeconds(5.0);
        auto Difference = LaterTime.GetSeconds().GetCount() - BaseTime.GetSeconds().GetCount();
        REQUIRE(Difference == Approx(5.0));
    }

    SECTION("Subtraction with duration creates earlier time point")
    {
        auto BaseTime = FTimePoint::FromDuration(FSeconds(15.0));
        auto EarlierTime = BaseTime - FSeconds(7.0);
        auto Difference = BaseTime.GetSeconds().GetCount() - EarlierTime.GetSeconds().GetCount();
        REQUIRE(Difference == Approx(7.0));
    }

    SECTION("Addition assignment modifies time point correctly")
    {
        auto TestTimePoint = FTimePoint::FromDuration(FSeconds(5.0));
        auto OriginalSeconds = TestTimePoint.GetSeconds().GetCount();
        TestTimePoint += FSeconds(3.0);
        REQUIRE(TestTimePoint.GetSeconds().GetCount() == Approx(OriginalSeconds + 3.0));
    }

    SECTION("Subtraction assignment modifies time point correctly")
    {
        auto TestTimePoint = FTimePoint::FromDuration(FSeconds(10.0));
        auto OriginalSeconds = TestTimePoint.GetSeconds().GetCount();
        TestTimePoint -= FSeconds(4.0);
        REQUIRE(TestTimePoint.GetSeconds().GetCount() == Approx(OriginalSeconds - 4.0));
    }

    SECTION("Equality operator returns true for identical time points")
    {
        auto FirstTime = FTimePoint::FromDuration(FSeconds(5.0));
        auto SecondTime = FTimePoint::FromDuration(FSeconds(5.0));
        REQUIRE((FirstTime == SecondTime) == true);
    }

    SECTION("Equality operator returns false for different time points")
    {
        auto FirstTime = FTimePoint::FromDuration(FSeconds(5.0));
        auto SecondTime = FTimePoint::FromDuration(FSeconds(8.0));
        REQUIRE((FirstTime == SecondTime) == false);
    }

    SECTION("Less than operator returns true for earlier time")
    {
        auto EarlierTime = FTimePoint::FromDuration(FSeconds(3.0));
        auto LaterTime = FTimePoint::FromDuration(FSeconds(7.0));
        REQUIRE((EarlierTime < LaterTime) == true);
    }

    SECTION("Greater than operator returns true for later time")
    {
        auto EarlierTime = FTimePoint::FromDuration(FSeconds(3.0));
        auto LaterTime = FTimePoint::FromDuration(FSeconds(7.0));
        REQUIRE((LaterTime > EarlierTime) == true);
    }

    SECTION("Less than or equal operator works correctly for time points")
    {
        auto FirstTime = FTimePoint::FromDuration(FSeconds(4.0));
        auto SecondTime = FTimePoint::FromDuration(FSeconds(4.0));
        auto EarlierTime = FTimePoint::FromDuration(FSeconds(2.0));
        REQUIRE((FirstTime <= SecondTime) == true);
        REQUIRE((EarlierTime <= FirstTime) == true);
    }

    SECTION("Now generates time point with positive ticks")
    {
        auto CurrentTime = FTimePoint::Now();
        REQUIRE(CurrentTime.GetTicks() > 0);
    }

    SECTION("Now generates different time points when called multiple times")
    {
        auto FirstTime = FTimePoint::Now();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        auto SecondTime = FTimePoint::Now();
        REQUIRE(SecondTime.GetTicks() > FirstTime.GetTicks());
    }

    SECTION("Time difference between Now calls is reasonable")
    {
        auto FirstTime = FTimePoint::Now();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        auto SecondTime = FTimePoint::Now();
        auto DifferenceMs = SecondTime.GetMilliSeconds().GetCount() - FirstTime.GetMilliSeconds().GetCount();
        REQUIRE(DifferenceMs >= 5);
        REQUIRE(DifferenceMs < 1000);
    }

    SECTION("GetNativeTimePoint returns valid std::chrono::time_point")
    {
        auto TestTimePoint = FTimePoint::Now();
        auto NativeTimePoint = TestTimePoint.GetNativeTimePoint();
        REQUIRE(NativeTimePoint.time_since_epoch().count() > 0);
    }

    SECTION("GetTicks returns non-zero value for non-epoch time")
    {
        auto TestTimePoint = FTimePoint::FromDuration(FSeconds(1.0));
        REQUIRE(TestTimePoint.GetTicks() != 0);
    }
}

TEST_CASE("Duration Concept", "[CDuration]")
{
    SECTION("FNanoSeconds satisfies CDuration concept")
    {
        static_assert(CDuration<FNanoSeconds>);
        REQUIRE(true);
    }

    SECTION("FMicroSeconds satisfies CDuration concept")
    {
        static_assert(CDuration<FMicroSeconds>);
        REQUIRE(true);
    }

    SECTION("FMilliSeconds satisfies CDuration concept")
    {
        static_assert(CDuration<FMilliSeconds>);
        REQUIRE(true);
    }

    SECTION("FSeconds satisfies CDuration concept")
    {
        static_assert(CDuration<FSeconds>);
        REQUIRE(true);
    }

    SECTION("FMinutes satisfies CDuration concept")
    {
        static_assert(CDuration<FMinutes>);
        REQUIRE(true);
    }

    SECTION("FHours satisfies CDuration concept")
    {
        static_assert(CDuration<FHours>);
        REQUIRE(true);
    }
}

TEST_CASE("Integration Tests", "[Integration]")
{
    SECTION("Time point arithmetic with multiple duration types produces correct result")
    {
        auto StartTime = FTimePoint::Now();
        auto AfterMs = StartTime + FMilliSeconds(500);
        auto AfterSec = AfterMs + FSeconds(1.0);
        auto AfterMin = AfterSec + FMinutes(0.5);
        auto TotalDiff = AfterMin.GetSeconds().GetCount() - StartTime.GetSeconds().GetCount();
        REQUIRE(TotalDiff == Approx(31.5));
    }

    SECTION("Round trip conversion maintains precision within acceptable bounds")
    {
        FNanoSeconds OriginalNano(1500000000);
        FSeconds ConvertedSeconds(OriginalNano);
        FNanoSeconds BackToNano(ConvertedSeconds);
        auto Difference = std::abs(BackToNano.GetCount() - OriginalNano.GetCount());
        REQUIRE(Difference < 1000);
    }

    SECTION("Mixed type duration operations work correctly")
    {
        FMilliSeconds Ms(2500);
        FSeconds Sec(1.5);
        auto Combined = Ms + Sec;
        REQUIRE(Combined.GetCount() == Approx(4.0));
    }

    SECTION("Complex time point manipulation produces expected result")
    {
        auto BaseTime = FTimePoint::FromDuration(FSeconds(100.0));
        BaseTime += FMinutes(2.0);
        BaseTime -= FMilliSeconds(5000);
        BaseTime += FHours(0.5);
        auto FinalSeconds = BaseTime.GetSeconds().GetCount();
        REQUIRE(FinalSeconds == Approx(2015.0));
    }
}
