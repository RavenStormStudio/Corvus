#include "Engine/Core/Crash/Assertion.hpp"

#include "Engine/Core/Crash/CrashHandler.hpp"

FAssertionData::FAssertionData(const FString& InCondition, const HRESULT InResultCode, const FString& InMessage, const FSourceLocation& InSourceLocation)
    : ResultCode(InResultCode), Condition(InCondition), Message(InMessage), SourceLocation(InSourceLocation)
{
}

void OnAssertion(const FAssertionData& AssertionData)
{
    ULONG_PTR Arguments[] = {reinterpret_cast<ULONG_PTR>(&AssertionData)};
    RaiseException(EXCEPTION_ASSERTION, 0, _countof(Arguments), Arguments);
}
