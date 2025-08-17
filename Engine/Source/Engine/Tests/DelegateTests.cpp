#include <catch2/catch_test_macros.hpp>
#include "Engine/Core/Utility/Delegates.hpp"

class TestObject
{
public:
    int32 Value = 0;

    void SetValue(const int32 NewValue)
    {
        Value = NewValue;
    }

    int32 GetValue() const
    {
        return Value;
    }

    void AddToValue(const int32 Amount)
    {
        Value += Amount;
    }

    bool8 IsPositive() const
    {
        return Value > 0;
    }

    void MultiParam(const int32 A, const float32 B, const bool8 C)
    {
        Value = A + static_cast<int32>(B) + (C ? 1 : 0);
    }

    int32 CalculateSum(const int32 A, const int32 B) const
    {
        return A + B + Value;
    }
};

static int32 GlobalCounter = 0;

void StaticIncrement()
{
    GlobalCounter++;
}

void StaticSetCounter(const int32 NewValue)
{
    GlobalCounter = NewValue;
}

int32 StaticGetCounter()
{
    return GlobalCounter;
}

bool8 StaticIsCounterPositive()
{
    return GlobalCounter > 0;
}

void StaticMultiParam(const int32 A, const float32 B, const bool8 C)
{
    GlobalCounter = A + static_cast<int32>(B) + (C ? 1 : 0);
}

int32 StaticCalculate(const int32 A, const int32 B)
{
    return A * B + GlobalCounter;
}

TEST_CASE("FDelegateHandle", "[DelegateHandle]")
{
    SECTION("Default handle is not valid")
    {
        FDelegateHandle Handle;
        REQUIRE(Handle.IsValid() == false);
        REQUIRE(Handle.GetId() == 0);
    }

    SECTION("NewHandle creates valid handle")
    {
        FDelegateHandle Handle = FDelegateHandle::NewHandle();
        REQUIRE(Handle.IsValid() == true);
        REQUIRE(Handle.GetId() != 0);
    }

    SECTION("NewHandle creates unique handles")
    {
        FDelegateHandle Handle1 = FDelegateHandle::NewHandle();
        FDelegateHandle Handle2 = FDelegateHandle::NewHandle();

        REQUIRE(Handle1.GetId() != Handle2.GetId());
        REQUIRE(Handle1 != Handle2);
    }

    SECTION("Reset makes handle invalid")
    {
        FDelegateHandle Handle = FDelegateHandle::NewHandle();
        REQUIRE(Handle.IsValid() == true);

        Handle.Reset();
        REQUIRE(Handle.IsValid() == false);
        REQUIRE(Handle.GetId() == 0);
    }

    SECTION("Copy constructor works correctly")
    {
        FDelegateHandle Original = FDelegateHandle::NewHandle();
        FDelegateHandle Copy(Original);

        REQUIRE(Copy.GetId() == Original.GetId());
        REQUIRE(Copy == Original);
    }

    SECTION("Move constructor works correctly")
    {
        FDelegateHandle Original = FDelegateHandle::NewHandle();
        uint64 OriginalId = Original.GetId();

        FDelegateHandle Moved(std::move(Original));

        REQUIRE(Moved.GetId() == OriginalId);
        REQUIRE(Moved.IsValid() == true);
    }

    SECTION("Comparison operators work correctly")
    {
        FDelegateHandle Handle1 = FDelegateHandle::NewHandle();
        FDelegateHandle Handle2 = FDelegateHandle::NewHandle();
        FDelegateHandle Handle1Copy = Handle1;

        REQUIRE(Handle1 == Handle1Copy);
        REQUIRE(Handle1 != Handle2);
        REQUIRE((Handle1 < Handle2 || Handle1 > Handle2));
    }
}

TEST_CASE("TDelegate - Basic Functionality", "[Delegate]")
{
    SECTION("Default delegate is not bound")
    {
        TDelegate<void()> TestDelegate;
        REQUIRE(TestDelegate.IsBound() == false);
    }

    SECTION("Static function binding")
    {
        GlobalCounter = 0;
        TDelegate<void()> TestDelegate;
        TestDelegate.Bind(StaticIncrement);

        REQUIRE(TestDelegate.IsBound() == true);
        TestDelegate.Execute();
        REQUIRE(GlobalCounter == 1);
    }

    SECTION("Static function with parameters")
    {
        GlobalCounter = 0;
        TDelegate<void(int32)> TestDelegate;
        TestDelegate.Bind(StaticSetCounter);

        TestDelegate.Execute(42);
        REQUIRE(GlobalCounter == 42);
    }

    SECTION("Static function with return value")
    {
        GlobalCounter = 25;
        TDelegate<int32()> TestDelegate;
        TestDelegate.Bind(StaticGetCounter);

        int32 Result = TestDelegate.Execute();
        REQUIRE(Result == 25);
    }

    SECTION("Member function binding")
    {
        TestObject Object;
        TDelegate<void(int32)> TestDelegate;
        TestDelegate.BindMember(&Object, &TestObject::SetValue);

        REQUIRE(TestDelegate.IsBound() == true);
        TestDelegate.Execute(100);
        REQUIRE(Object.Value == 100);
    }

    SECTION("Const member function binding")
    {
        TestObject Object;
        Object.Value = 75;

        TDelegate<int32()> TestDelegate;
        TestDelegate.BindMember(&Object, &TestObject::GetValue);

        int32 Result = TestDelegate.Execute();
        REQUIRE(Result == 75);
    }

    SECTION("Lambda binding")
    {
        int32 CapturedValue = 0;
        TDelegate<void(int32)> TestDelegate;

        TestDelegate.Bind([&CapturedValue](const int32 Value)
        {
            CapturedValue = Value * 2;
        });

        TestDelegate.Execute(21);
        REQUIRE(CapturedValue == 42);
    }

    SECTION("Lambda with return value")
    {
        TDelegate<int32(int32, int32)> TestDelegate;

        TestDelegate.Bind([](const int32 A, const int32 B) -> int32
        {
            return A * B;
        });

        int32 Result = TestDelegate.Execute(6, 7);
        REQUIRE(Result == 42);
    }

    SECTION("Functor binding")
    {
        struct Multiplier
        {
            int32 Factor;

            int32 operator()(const int32 Value) const
            {
                return Value * Factor;
            }
        };

        Multiplier Mult{5};
        TDelegate<int32(int32)> TestDelegate;
        TestDelegate.Bind(Mult);

        int32 Result = TestDelegate.Execute(8);
        REQUIRE(Result == 40);
    }

    SECTION("Unbind delegate")
    {
        TDelegate<void()> TestDelegate;
        TestDelegate.Bind(StaticIncrement);

        REQUIRE(TestDelegate.IsBound() == true);
        TestDelegate.Unbind();
        REQUIRE(TestDelegate.IsBound() == false);
    }

    SECTION("Execute unbound delegate with return value returns default")
    {
        TDelegate<int32()> TestDelegate;

        int32 Result = TestDelegate.Execute();
        REQUIRE(Result == 0);
    }

    SECTION("Execute unbound void delegate does nothing")
    {
        GlobalCounter = 10;
        TDelegate<void()> TestDelegate;

        TestDelegate.Execute();
        REQUIRE(GlobalCounter == 10);
    }

    SECTION("Multiple parameters")
    {
        GlobalCounter = 0;
        TDelegate<void(int32, float32, bool8)> TestDelegate;
        TestDelegate.Bind(StaticMultiParam);

        TestDelegate.Execute(10, 5.5f, true);
        REQUIRE(GlobalCounter == 16);
    }
}

TEST_CASE("TDelegate - Copy and Move", "[Delegate]")
{
    SECTION("Copy constructor")
    {
        GlobalCounter = 0;
        TDelegate<void()> Original;
        Original.Bind(StaticIncrement);

        TDelegate<void()> Copy(Original);

        REQUIRE(Copy.IsBound() == true);
        Copy.Execute();
        REQUIRE(GlobalCounter == 1);
    }

    SECTION("Move constructor")
    {
        GlobalCounter = 0;
        TDelegate<void()> Original;
        Original.Bind(StaticIncrement);

        TDelegate<void()> Moved(std::move(Original));

        REQUIRE(Moved.IsBound() == true);
        Moved.Execute();
        REQUIRE(GlobalCounter == 1);
    }

    SECTION("Copy assignment")
    {
        GlobalCounter = 0;
        TDelegate<void()> Original;
        Original.Bind(StaticIncrement);

        TDelegate<void()> Copy;
        Copy = Original;

        REQUIRE(Copy.IsBound() == true);
        Copy.Execute();
        REQUIRE(GlobalCounter == 1);
    }

    SECTION("Move assignment")
    {
        GlobalCounter = 0;
        TDelegate<void()> Original;
        Original.Bind(StaticIncrement);

        TDelegate<void()> Moved;
        Moved = std::move(Original);

        REQUIRE(Moved.IsBound() == true);
        Moved.Execute();
        REQUIRE(GlobalCounter == 1);
    }
}

TEST_CASE("TMulticastDelegate - Basic Functionality", "[MulticastDelegate]")
{
    SECTION("Default multicast delegate has no bindings")
    {
        TMulticastDelegate<void()> TestDelegate;
        REQUIRE(TestDelegate.IsBound() == false);
        REQUIRE(TestDelegate.GetBindingsCount() == 0);
    }

    SECTION("Add static function")
    {
        GlobalCounter = 0;
        TMulticastDelegate<void()> TestDelegate;

        FDelegateHandle Handle = TestDelegate.AddBinding(StaticIncrement);

        REQUIRE(Handle.IsValid() == true);
        REQUIRE(TestDelegate.IsBound() == true);
        REQUIRE(TestDelegate.GetBindingsCount() == 1);
        REQUIRE(TestDelegate.IsHandleBound(Handle) == true);

        TestDelegate.Broadcast();
        REQUIRE(GlobalCounter == 1);
    }

    SECTION("Add multiple static functions")
    {
        GlobalCounter = 0;
        TMulticastDelegate<void()> TestDelegate;

        FDelegateHandle Handle1 = TestDelegate.AddBinding(StaticIncrement);
        FDelegateHandle Handle2 = TestDelegate.AddBinding(StaticIncrement);
        FDelegateHandle Handle3 = TestDelegate.AddBinding(StaticIncrement);

        REQUIRE(TestDelegate.GetBindingsCount() == 3);
        REQUIRE(TestDelegate.IsHandleBound(Handle1) == true);
        REQUIRE(TestDelegate.IsHandleBound(Handle2) == true);
        REQUIRE(TestDelegate.IsHandleBound(Handle3) == true);

        TestDelegate.Broadcast();
        REQUIRE(GlobalCounter == 3);
    }

    SECTION("Add member functions")
    {
        TestObject Object1, Object2;
        TMulticastDelegate<void(int32)> TestDelegate;

        FDelegateHandle Handle1 = TestDelegate.AddMemberBinding(&Object1, &TestObject::SetValue);
        FDelegateHandle Handle2 = TestDelegate.AddMemberBinding(&Object2, &TestObject::SetValue);

        REQUIRE(TestDelegate.GetBindingsCount() == 2);
        REQUIRE(TestDelegate.IsHandleBound(Handle1) == true);
        REQUIRE(TestDelegate.IsHandleBound(Handle2) == true);

        TestDelegate.Broadcast(50);

        REQUIRE(Object1.Value == 50);
        REQUIRE(Object2.Value == 50);
    }

    SECTION("Add const member functions")
    {
        TestObject Object1, Object2;
        Object1.Value = 10;
        Object2.Value = 20;

        int32 Sum = 0;
        TMulticastDelegate<void()> TestDelegate;

        FDelegateHandle Handle1 = TestDelegate.AddBinding([&Object1, &Sum]() { Sum += Object1.GetValue(); });
        FDelegateHandle Handle2 = TestDelegate.AddBinding([&Object2, &Sum]() { Sum += Object2.GetValue(); });

        REQUIRE(TestDelegate.IsHandleBound(Handle1) == true);
        REQUIRE(TestDelegate.IsHandleBound(Handle2) == true);

        TestDelegate.Broadcast();
        REQUIRE(Sum == 30);
    }

    SECTION("Add lambdas")
    {
        int32 Counter1 = 0, Counter2 = 0;
        TMulticastDelegate<void(int32)> TestDelegate;

        FDelegateHandle Handle1 = TestDelegate.AddBinding([&Counter1](const int32 Value) { Counter1 += Value; });
        FDelegateHandle Handle2 = TestDelegate.AddBinding([&Counter2](const int32 Value) { Counter2 += Value * 2; });

        REQUIRE(TestDelegate.IsHandleBound(Handle1) == true);
        REQUIRE(TestDelegate.IsHandleBound(Handle2) == true);

        TestDelegate.Broadcast(5);

        REQUIRE(Counter1 == 5);
        REQUIRE(Counter2 == 10);
    }

    SECTION("Add mixed callables")
    {
        GlobalCounter = 0;
        TestObject Object;
        int32 LambdaResult = 0;

        TMulticastDelegate<void(int32)> TestDelegate;

        FDelegateHandle Handle1 = TestDelegate.AddBinding(StaticSetCounter);
        FDelegateHandle Handle2 = TestDelegate.AddMemberBinding(&Object, &TestObject::SetValue);
        FDelegateHandle Handle3 = TestDelegate.AddBinding([&LambdaResult](const int32 Value) { LambdaResult = Value * 3; });

        REQUIRE(TestDelegate.GetBindingsCount() == 3);
        REQUIRE(TestDelegate.IsHandleBound(Handle1) == true);
        REQUIRE(TestDelegate.IsHandleBound(Handle2) == true);
        REQUIRE(TestDelegate.IsHandleBound(Handle3) == true);

        TestDelegate.Broadcast(7);

        REQUIRE(GlobalCounter == 7);
        REQUIRE(Object.Value == 7);
        REQUIRE(LambdaResult == 21);
    }

    SECTION("Remove binding by handle")
    {
        GlobalCounter = 0;
        TMulticastDelegate<void()> TestDelegate;

        FDelegateHandle Handle1 = TestDelegate.AddBinding(StaticIncrement);
        FDelegateHandle Handle2 = TestDelegate.AddBinding(StaticIncrement);

        REQUIRE(TestDelegate.GetBindingsCount() == 2);

        bool8 Removed = TestDelegate.RemoveBinding(Handle1);

        REQUIRE(Removed == true);
        REQUIRE(TestDelegate.GetBindingsCount() == 1);
        REQUIRE(TestDelegate.IsHandleBound(Handle1) == false);
        REQUIRE(TestDelegate.IsHandleBound(Handle2) == true);
        REQUIRE(Handle1.IsValid() == false);

        TestDelegate.Broadcast();
        REQUIRE(GlobalCounter == 1);
    }

    SECTION("Remove non-existent binding returns false")
    {
        TMulticastDelegate<void()> TestDelegate;
        FDelegateHandle InvalidHandle = FDelegateHandle::NewHandle();

        bool8 Removed = TestDelegate.RemoveBinding(InvalidHandle);
        REQUIRE(Removed == false);
    }

    SECTION("Clear all bindings")
    {
        GlobalCounter = 0;
        TMulticastDelegate<void()> TestDelegate;

        FDelegateHandle Handle1 = TestDelegate.AddBinding(StaticIncrement);
        FDelegateHandle Handle2 = TestDelegate.AddBinding(StaticIncrement);
        FDelegateHandle Handle3 = TestDelegate.AddBinding(StaticIncrement);

        REQUIRE(TestDelegate.GetBindingsCount() == 3);
        REQUIRE(TestDelegate.IsHandleBound(Handle1) == true);
        REQUIRE(TestDelegate.IsHandleBound(Handle2) == true);
        REQUIRE(TestDelegate.IsHandleBound(Handle3) == true);

        TestDelegate.Clear();

        REQUIRE(TestDelegate.GetBindingsCount() == 0);
        REQUIRE(TestDelegate.IsBound() == false);
        REQUIRE(TestDelegate.IsHandleBound(Handle1) == false);
        REQUIRE(TestDelegate.IsHandleBound(Handle2) == false);
        REQUIRE(TestDelegate.IsHandleBound(Handle3) == false);

        TestDelegate.Broadcast();
        REQUIRE(GlobalCounter == 0);
    }

    SECTION("Empty multicast delegate broadcast does nothing")
    {
        GlobalCounter = 10;
        TMulticastDelegate<void()> TestDelegate;

        TestDelegate.Broadcast();
        REQUIRE(GlobalCounter == 10);
    }

    SECTION("Multiple parameters")
    {
        GlobalCounter = 0;
        TestObject Object;

        TMulticastDelegate<void(int32, float32, bool8)> TestDelegate;

        FDelegateHandle Handle1 = TestDelegate.AddBinding(StaticMultiParam);
        FDelegateHandle Handle2 = TestDelegate.AddMemberBinding(&Object, &TestObject::MultiParam);

        REQUIRE(TestDelegate.IsHandleBound(Handle1) == true);
        REQUIRE(TestDelegate.IsHandleBound(Handle2) == true);

        TestDelegate.Broadcast(15, 3.7f, true);

        REQUIRE(GlobalCounter == 19);
        REQUIRE(Object.Value == 19);
    }
}

TEST_CASE("TMulticastDelegate - Move Semantics", "[MulticastDelegate]")
{
    SECTION("Move constructor")
    {
        GlobalCounter = 0;
        TMulticastDelegate<void()> Original;
        FDelegateHandle Handle1 = Original.AddBinding(StaticIncrement);
        FDelegateHandle Handle2 = Original.AddBinding(StaticIncrement);

        TMulticastDelegate<void()> Moved(std::move(Original));

        REQUIRE(Moved.GetBindingsCount() == 2);
        REQUIRE(Moved.IsHandleBound(Handle1) == true);
        REQUIRE(Moved.IsHandleBound(Handle2) == true);

        Moved.Broadcast();
        REQUIRE(GlobalCounter == 2);
    }

    SECTION("Move assignment")
    {
        GlobalCounter = 0;
        TMulticastDelegate<void()> Original;
        FDelegateHandle Handle1 = Original.AddBinding(StaticIncrement);
        FDelegateHandle Handle2 = Original.AddBinding(StaticIncrement);

        TMulticastDelegate<void()> Moved;
        Moved = std::move(Original);

        REQUIRE(Moved.GetBindingsCount() == 2);
        REQUIRE(Moved.IsHandleBound(Handle1) == true);
        REQUIRE(Moved.IsHandleBound(Handle2) == true);

        Moved.Broadcast();
        REQUIRE(GlobalCounter == 2);
    }
}

TEST_CASE("Delegate Macros", "[DelegateMacros]")
{
    SECTION("DECLARE_DELEGATE creates void delegate")
    {
        DECLARE_DELEGATE(FTest);

        GlobalCounter = 0;
        FTestDelegate TestDelegate;
        TestDelegate.Bind(StaticIncrement);

        TestDelegate.Execute();
        REQUIRE(GlobalCounter == 1);
    }

    SECTION("DECLARE_DELEGATE with parameters")
    {
        DECLARE_DELEGATE(FTestParam, int32);

        GlobalCounter = 0;
        FTestParamDelegate TestDelegate;
        TestDelegate.Bind(StaticSetCounter);

        TestDelegate.Execute(42);
        REQUIRE(GlobalCounter == 42);
    }

    SECTION("DECLARE_DELEGATE_RETURN creates delegate with return type")
    {
        DECLARE_DELEGATE_RETURN(FTestReturn, int32);

        GlobalCounter = 25;
        FTestReturnDelegate TestDelegate;
        TestDelegate.Bind(StaticGetCounter);

        int32 Result = TestDelegate.Execute();
        REQUIRE(Result == 25);
    }

    SECTION("DECLARE_DELEGATE_RETURN with parameters")
    {
        DECLARE_DELEGATE_RETURN(FTestReturnParam, int32, int32, int32);

        GlobalCounter = 5;
        FTestReturnParamDelegate TestDelegate;
        TestDelegate.Bind(StaticCalculate);

        int32 Result = TestDelegate.Execute(3, 4);
        REQUIRE(Result == 17);
    }

    SECTION("DECLARE_MULTICAST_DELEGATE creates multicast delegate")
    {
        DECLARE_MULTICAST_DELEGATE(FTestMulti);

        GlobalCounter = 0;
        FTestMultiDelegate TestDelegate;
        FDelegateHandle Handle1 = TestDelegate.AddBinding(StaticIncrement);
        FDelegateHandle Handle2 = TestDelegate.AddBinding(StaticIncrement);

        REQUIRE(TestDelegate.IsHandleBound(Handle1) == true);
        REQUIRE(TestDelegate.IsHandleBound(Handle2) == true);

        TestDelegate.Broadcast();
        REQUIRE(GlobalCounter == 2);
    }

    SECTION("DECLARE_MULTICAST_DELEGATE with parameters")
    {
        DECLARE_MULTICAST_DELEGATE(FTestMultiParam, int32);

        GlobalCounter = 0;
        FTestMultiParamDelegate TestDelegate;
        FDelegateHandle Handle1 = TestDelegate.AddBinding(StaticSetCounter);
        FDelegateHandle Handle2 = TestDelegate.AddBinding([](const int32 Value) { GlobalCounter += Value; });

        REQUIRE(TestDelegate.IsHandleBound(Handle1) == true);
        REQUIRE(TestDelegate.IsHandleBound(Handle2) == true);

        TestDelegate.Broadcast(10);
        REQUIRE(GlobalCounter == 20);
    }
}

TEST_CASE("Delegate Edge Cases", "[DelegateEdgeCases]")
{
    SECTION("Delegate with lambda that captures by reference")
    {
        int32 CapturedValue = 100;
        TDelegate<int32()> TestDelegate;

        TestDelegate.Bind([&CapturedValue]() -> int32
        {
            return CapturedValue * 2;
        });

        int32 Result = TestDelegate.Execute();
        REQUIRE(Result == 200);

        CapturedValue = 50;
        Result = TestDelegate.Execute();
        REQUIRE(Result == 100);
    }

    SECTION("Multicast delegate with lambda that returns value")
    {
        TMulticastDelegate<void()> TestDelegate;

        FDelegateHandle Handle = TestDelegate.AddBinding([]() -> int32
        {
            GlobalCounter++;
            return 42;
        });

        REQUIRE(TestDelegate.IsHandleBound(Handle) == true);

        GlobalCounter = 0;
        TestDelegate.Broadcast();
        REQUIRE(GlobalCounter == 1);
    }

    SECTION("Rebinding delegate overwrites previous binding")
    {
        GlobalCounter = 0;
        TDelegate<void()> TestDelegate;

        TestDelegate.Bind(StaticIncrement);
        TestDelegate.Execute();
        REQUIRE(GlobalCounter == 1);

        TestDelegate.Bind([]() { GlobalCounter += 10; });
        TestDelegate.Execute();
        REQUIRE(GlobalCounter == 11);
    }
}
