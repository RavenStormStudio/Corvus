#include "Engine/Engine/Engine.hpp"

int main()
{
    GetEngine()->Initialize();
    GetEngine()->Shutdown();
    FEngine::DestroyInstance();
    return 0;
}
