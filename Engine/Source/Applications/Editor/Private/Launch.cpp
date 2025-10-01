// RavenStorm Copyright @ 2025-2025

#if defined(CORVUS_MODE_DEBUG) || defined(CORVUS_MODE_DEVELOPMENT)
int main()
#else
#include <Windows.h>

int APIENTRY WinMain(HINSTANCE Instance, HINSTANCE PreviousInstance, PSTR CommandLine, int ShowCommand)
#endif
{
    return 0;
}
