#pragma once

int TestMain(int ArgumentCount, char* Arguments[]);

#define LAUNCH_TESTS() \
    int main(int ArgumentCount, char* Arguments[]) \
    { \
        return TestMain(ArgumentCount, Arguments); \
    }