// RavenStorm Copyright @ 2025-2025

#include <catch2/catch_session.hpp>

int main(const int ArgumentCount, char* Arguments[])
{
    Catch::Session Session = Catch::Session();
    int Result = Session.applyCommandLine(ArgumentCount, Arguments);
    if (Result != 0)
    {
        return Result;
    }
    Result = Session.run(ArgumentCount, Arguments);
    return Result;
}
