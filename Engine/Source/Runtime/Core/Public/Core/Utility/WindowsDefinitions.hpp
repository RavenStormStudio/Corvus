// RavenStorm Copyright @ 2025-2025

#pragma once

struct HINSTANCE__;
struct HWND__;
struct HKEY__;
struct HMONITOR__;
struct _EXCEPTION_POINTERS;
struct tagRECT;

namespace Windows
{
    using LONG = long;
    using DWORD = unsigned long;
    using HRESULT = long;

    using HANDLE = void*;
    using HINSTANCE = HINSTANCE__*;
    using HWND = HWND__*;
    using HMONITOR = HMONITOR__*;
    using RECT = tagRECT;
    using HMODULE = HINSTANCE;

    using EXCEPTION_POINTERS = _EXCEPTION_POINTERS;

    using HKEY = HKEY__*;

    using LRESULT = int64;
    using WPARAM = uint64;
    using LPARAM = int64;
}

#define CREATE_HRESULT(sev,fac,code) ((Windows::HRESULT) (((unsigned long)(sev)<<31) | ((unsigned long)(fac)<<16) | ((unsigned long)(code))) )
