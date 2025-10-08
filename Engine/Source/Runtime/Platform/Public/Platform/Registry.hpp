// RavenStorm Copyright @ 2025-2025

#pragma once

#include <Windows.h>

#include "Core/CoreDefinitions.hpp"
#include "Core/Containers/Array.hpp"
#include "Core/Containers/String.hpp"

enum class PLATFORM_API ERegistryHive : uint32
{
    Unknown = 0,
    ClassesRoot = 0x80000000,
    CurrentUser = 0x80000001,
    CurrentConfig = 0x80000005,
    LocalMachine = 0x80000002,
    Users = 0x80000003,
};

enum class PLATFORM_API ERegistryValueType : uint8
{
    None = REG_NONE,
    String = REG_SZ,
    ExpandString = REG_EXPAND_SZ,
    Binary = REG_BINARY,
    DWord = REG_DWORD,
    QWord = REG_QWORD,
    MultiString = REG_MULTI_SZ,
};

struct PLATFORM_API FRegistryValue
{
public:
    FString Name;
    ERegistryValueType Type = ERegistryValueType::None;
    TArray<uint8> Data;

public:
    [[nodiscard]] FString AsString() const;
    [[nodiscard]] uint32 AsDWord() const;
    [[nodiscard]] uint64 AsQWord() const;
    [[nodiscard]] TArray<FString> AsMultiString() const;
    [[nodiscard]] TArray<uint8> AsBinary() const;
};

struct PLATFORM_API FRegistryKey
{
public:
    FRegistryKey() = default;
    FRegistryKey(ERegistryHive InHive, FString InSubKey);
    ~FRegistryKey();

    NON_COPYABLE(FRegistryKey)
    DEFAULT_MOVEABLE(FRegistryKey)

public:
    [[nodiscard]] bool8 IsValid() const noexcept;
    [[nodiscard]] bool8 Exists() const noexcept;

    bool8 Create();
    bool8 Open(bool8 ReadOnly = false);
    void Close();
    bool8 Delete();

    [[nodiscard]] bool8 HasValue(const FString& ValueName) const;
    [[nodiscard]] FRegistryValue ReadValue(const FString& ValueName) const;
    bool8 WriteValue(const FString& ValueName, const FRegistryValue& Value) const;
    bool8 DeleteValue(const FString& ValueName) const;

    bool8 WriteString(const FString& ValueName, const FString& Value) const;
    bool8 WriteDWord(const FString& ValueName, uint32 Value) const;
    bool8 WriteQWord(const FString& ValueName, uint64 Value) const;
    bool8 WriteBinary(const FString& ValueName, const TArray<uint8>& Value) const;
    bool8 WriteMultiString(const FString& ValueName, const TArray<FString>& Values) const;

    [[nodiscard]] FString ReadString(const FString& ValueName, const FString& DefaultValue = {}) const;
    [[nodiscard]] uint32 ReadDWord(const FString& ValueName, uint32 DefaultValue = 0) const;
    [[nodiscard]] uint64 ReadQWord(const FString& ValueName, uint64 DefaultValue = 0) const;
    [[nodiscard]] TArray<uint8> ReadBinary(const FString& ValueName) const;
    [[nodiscard]] TArray<FString> ReadMultiString(const FString& ValueName) const;

    [[nodiscard]] TArray<FString> GetSubKeyNames() const;
    [[nodiscard]] TArray<FString> GetValueNames() const;
    [[nodiscard]] TArray<FRegistryValue> GetValues() const;

    [[nodiscard]] FString GetFullPath() const;

public:
    [[nodiscard]] ERegistryHive GetHive() const { return Hive; }
    [[nodiscard]] FString GetSubKey() const { return SubKey; }

private:
    [[nodiscard]] HKEY GetHiveHandle() const;

private:
    ERegistryHive Hive = ERegistryHive::Unknown;
    FString SubKey;
    HKEY KeyHandle = nullptr;
    bool8 IsOpen = false;
};

class PLATFORM_API FRegistry
{
public:
    [[nodiscard]] static bool8 KeyExists(ERegistryHive Hive, const FString& SubKey);

    static bool8 CreateKey(ERegistryHive Hive, const FString& SubKey);
    static bool8 DeleteKey(ERegistryHive Hive, const FString& SubKey);
    static bool8 DeleteKeyTree(ERegistryHive Hive, const FString& SubKey);

    [[nodiscard]] static FString ReadString(ERegistryHive Hive, const FString& SubKey, const FString& ValueName, const FString& DefaultValue = {});
    [[nodiscard]] static uint32 ReadDWord(ERegistryHive Hive, const FString& SubKey, const FString& ValueName, uint32 DefaultValue = 0);
    [[nodiscard]] static uint64 ReadQWord(ERegistryHive Hive, const FString& SubKey, const FString& ValueName, uint64 DefaultValue = 0);

    static bool8 WriteString(ERegistryHive Hive, const FString& SubKey, const FString& ValueName, const FString& Value);
    static bool8 WriteDWord(ERegistryHive Hive, const FString& SubKey, const FString& ValueName, uint32 Value);
    static bool8 WriteQWord(ERegistryHive Hive, const FString& SubKey, const FString& ValueName, uint64 Value);
};
