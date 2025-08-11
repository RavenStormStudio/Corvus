#pragma once

#include "Engine/Core/Memory/SmartPointers.hpp"

template <typename T>
class TSingleton
{
private:
    TSingleton() = default;

public:
    [[nodiscard]] static TSharedPtr<T> GetInstance()
    {
        if (Instance == nullptr)
        {
            Instance = MakeShared<T>();
        }
        return Instance;
    }

    static void DestroyInstance()
    {
        Instance.reset();
    }

private:
    inline static TSharedPtr<T> Instance = nullptr;

    friend T;
};
