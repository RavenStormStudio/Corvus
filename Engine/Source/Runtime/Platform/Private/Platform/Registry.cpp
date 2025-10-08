// RavenStorm Copyright @ 2025-2025

#include "Platform/Registry.hpp"

#include "Core/Assertion/Assertion.hpp"
#include "Core/Logging/LogManager.hpp"

DEFINE_LOG_CHANNEL(Registry, All)

namespace
{
    HKEY GetHiveHandle(const ERegistryHive Hive)
    {
        switch (Hive)
        {
        case ERegistryHive::ClassesRoot: return HKEY_CLASSES_ROOT;
        case ERegistryHive::CurrentUser: return HKEY_CURRENT_USER;
        case ERegistryHive::CurrentConfig: return HKEY_CURRENT_CONFIG;
        case ERegistryHive::LocalMachine: return HKEY_LOCAL_MACHINE;
        case ERegistryHive::Users: return HKEY_USERS;
        case ERegistryHive::Unknown:
            break;
        }
        return nullptr;
    }

    FString GetHiveName(const ERegistryHive Hive)
    {
        switch (Hive)
        {
        case ERegistryHive::ClassesRoot: return TEXT("HKEY_CLASSES_ROOT");
        case ERegistryHive::CurrentUser: return TEXT("HKEY_CURRENT_USER");
        case ERegistryHive::CurrentConfig: return TEXT("HKEY_CURRENT_CONFIG");
        case ERegistryHive::LocalMachine: return TEXT("HKEY_LOCAL_MACHINE");
        case ERegistryHive::Users: return TEXT("HKEY_USERS");
        case ERegistryHive::Unknown:
            break;
        }
        return TEXT("Unknown");
    }
}

FString FRegistryValue::AsString() const
{
    if (Type == ERegistryValueType::String || Type == ERegistryValueType::ExpandString)
    {
        return {reinterpret_cast<const FChar*>(Data.GetData()), Data.Num() / sizeof(FChar)};
    }
    return {};
}

uint32 FRegistryValue::AsDWord() const
{
    if (Type == ERegistryValueType::DWord && Data.Num() >= sizeof(uint32))
    {
        return *reinterpret_cast<const uint32*>(Data.GetData());
    }
    return 0;
}

uint64 FRegistryValue::AsQWord() const
{
    if (Type == ERegistryValueType::QWord && Data.Num() >= sizeof(uint64))
    {
        return *reinterpret_cast<const uint64*>(Data.GetData());
    }
    return 0;
}

TArray<FString> FRegistryValue::AsMultiString() const
{
    TArray<FString> Result;
    if (Type == ERegistryValueType::MultiString)
    {
        const FChar* Current = reinterpret_cast<const FChar*>(Data.GetData());
        const FChar* End = Current + (Data.Num() / sizeof(FChar));
        while (Current < End && *Current)
        {
            FString String(Current);
            Current += String.length() + 1;
            Result.PushBack(std::move(String));
        }
    }
    return Result;
}

TArray<uint8> FRegistryValue::AsBinary() const
{
    return Data;
}

FRegistryKey::FRegistryKey(const ERegistryHive InHive, FString InSubKey)
    : Hive(InHive), SubKey(std::move(InSubKey))
{
}

FRegistryKey::~FRegistryKey()
{
    Close();
}

bool8 FRegistryKey::IsValid() const noexcept
{
    return Hive != ERegistryHive::Unknown && !SubKey.empty();
}

bool8 FRegistryKey::Exists() const noexcept
{
    if (!IsValid())
    {
        return false;
    }
    HKEY TempKey;
    const LONG Result = RegOpenKeyEx(GetHiveHandle(), SubKey.c_str(), 0, KEY_READ, &TempKey);
    if (Result == ERROR_SUCCESS)
    {
        RegCloseKey(TempKey);
        return true;
    }
    return false;
}

bool8 FRegistryKey::Create()
{
    if (!IsValid())
    {
        CVLOG(LogRegistry, Error, TEXT("Cannot create invalid registry key"));
        return false;
    }
    if (IsOpen)
    {
        Close();
    }
    DWORD Disposition;
    const LONG Result = RegCreateKeyEx(GetHiveHandle(), SubKey.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, nullptr, &KeyHandle, &Disposition);
    if (Result == ERROR_SUCCESS)
    {
        IsOpen = true;
        if (Disposition == REG_CREATED_NEW_KEY)
        {
            CVLOG(LogRegistry, Info, TEXT("Created new registry key: {}\\{}"), GetHiveName(Hive), SubKey);
        }
        return true;
    }
    CVLOG(LogRegistry, Error, TEXT("Failed to create registry key: {}\\{} (Error: {} -> \"{}\")"), GetHiveName(Hive), SubKey, Result, FExceptionHandler::GetResultDescription(Result));
    return false;
}

bool8 FRegistryKey::Open(const bool8 ReadOnly)
{
    if (!IsValid())
    {
        CVLOG(LogRegistry, Error, TEXT("Cannot open invalid registry key"));
        return false;
    }
    if (IsOpen)
    {
        return true;
    }
    const REGSAM Access = ReadOnly ? KEY_READ : (KEY_READ | KEY_WRITE);
    const LONG Result = RegOpenKeyEx(GetHiveHandle(), SubKey.c_str(), 0, Access, &KeyHandle);
    if (Result == ERROR_SUCCESS)
    {
        IsOpen = true;
        return true;
    }
    CVLOG(LogRegistry, Error, TEXT("Failed to open registry key: {}\\{} (Error: {})"), GetHiveName(Hive), SubKey, Result);
    return false;
}

void FRegistryKey::Close()
{
    if (IsOpen && KeyHandle)
    {
        RegCloseKey(KeyHandle);
        KeyHandle = nullptr;
        IsOpen = false;
    }
}

bool8 FRegistryKey::Delete()
{
    if (!IsValid())
    {
        return false;
    }
    Close();
    const LONG Result = RegDeleteKey(GetHiveHandle(), SubKey.c_str());
    return Result == ERROR_SUCCESS;
}

bool8 FRegistryKey::HasValue(const FString& ValueName) const
{
    if (!IsOpen)
    {
        return false;
    }
    const LONG Result = RegQueryValueEx(KeyHandle, ValueName.c_str(), nullptr, nullptr, nullptr, nullptr);
    return Result == ERROR_SUCCESS;
}

FRegistryValue FRegistryKey::ReadValue(const FString& ValueName) const
{
    FRegistryValue Value;
    Value.Name = ValueName;
    if (!IsOpen)
    {
        CVLOG(LogRegistry, Warning, TEXT("Attempting to read from closed registry key"));
        return Value;
    }
    DWORD Type = 0;
    DWORD DataSize = 0;
    LONG Result = RegQueryValueEx(KeyHandle, ValueName.c_str(), nullptr, &Type, nullptr, &DataSize);
    if (Result != ERROR_SUCCESS)
    {
        return Value;
    }
    Value.Type = static_cast<ERegistryValueType>(Type);
    Value.Data.Resize(DataSize);
    Result = RegQueryValueEx(KeyHandle, ValueName.c_str(), nullptr, &Type, Value.Data.GetData(), &DataSize);
    if (Result != ERROR_SUCCESS)
    {
        Value.Data.Clear();
        Value.Type = ERegistryValueType::None;
    }
    return Value;
}

bool8 FRegistryKey::WriteValue(const FString& ValueName, const FRegistryValue& Value) const
{
    if (!IsOpen)
    {
        CVLOG(LogRegistry, Warning, TEXT("Attempting to write to closed registry key"));
        return false;
    }
    const LONG Result = RegSetValueEx(KeyHandle, ValueName.c_str(), 0, static_cast<DWORD>(Value.Type), Value.Data.GetData(), static_cast<DWORD>(Value.Data.Num()));
    if (Result != ERROR_SUCCESS)
    {
        CVLOG(LogRegistry, Error, TEXT("Failed to write registry value: {} (Error: {})"), ValueName, Result);
        return false;
    }
    return true;
}

bool8 FRegistryKey::DeleteValue(const FString& ValueName) const
{
    if (!IsOpen)
    {
        return false;
    }
    const LONG Result = RegDeleteValue(KeyHandle, ValueName.c_str());
    return Result == ERROR_SUCCESS;
}

bool8 FRegistryKey::WriteString(const FString& ValueName, const FString& Value) const
{
    FRegistryValue RegValue;
    RegValue.Name = ValueName;
    RegValue.Type = ERegistryValueType::String;
    const size64 ByteSize = (Value.length() + 1) * sizeof(FChar);
    RegValue.Data.Resize(ByteSize);
    FMemory::Copy(Value.c_str(), RegValue.Data.GetData(), ByteSize);
    return WriteValue(ValueName, RegValue);
}

bool8 FRegistryKey::WriteDWord(const FString& ValueName, const uint32 Value) const
{
    FRegistryValue RegValue;
    RegValue.Name = ValueName;
    RegValue.Type = ERegistryValueType::DWord;
    RegValue.Data.Resize(sizeof(uint32));
    FMemory::Copy(&Value, RegValue.Data.GetData(), sizeof(uint32));
    return WriteValue(ValueName, RegValue);
}

bool8 FRegistryKey::WriteQWord(const FString& ValueName, const uint64 Value) const
{
    FRegistryValue RegValue;
    RegValue.Name = ValueName;
    RegValue.Type = ERegistryValueType::QWord;
    RegValue.Data.Resize(sizeof(uint64));
    FMemory::Copy(&Value, RegValue.Data.GetData(), sizeof(uint64));
    return WriteValue(ValueName, RegValue);
}

bool8 FRegistryKey::WriteBinary(const FString& ValueName, const TArray<uint8>& Value) const
{
    FRegistryValue RegValue;
    RegValue.Name = ValueName;
    RegValue.Type = ERegistryValueType::Binary;
    RegValue.Data = Value;
    return WriteValue(ValueName, RegValue);
}

bool8 FRegistryKey::WriteMultiString(const FString& ValueName, const TArray<FString>& Values) const
{
    FRegistryValue RegValue;
    RegValue.Name = ValueName;
    RegValue.Type = ERegistryValueType::MultiString;
    size64 TotalSize = sizeof(FChar);
    for (const auto& Str : Values)
    {
        TotalSize += (Str.length() + 1) * sizeof(FChar);
    }
    RegValue.Data.Resize(TotalSize);
    uint8* Current = RegValue.Data.GetData();
    for (const auto& Str : Values)
    {
        const size64 ByteSize = (Str.length() + 1) * sizeof(FChar);
        FMemory::Copy(Str.c_str(), Current, ByteSize);
        Current += ByteSize;
    }
    *reinterpret_cast<FChar*>(Current) = 0;
    return WriteValue(ValueName, RegValue);
}

FString FRegistryKey::ReadString(const FString& ValueName, const FString& DefaultValue) const
{
    const FRegistryValue Value = ReadValue(ValueName);
    if (Value.Type == ERegistryValueType::String || Value.Type == ERegistryValueType::ExpandString)
    {
        return Value.AsString();
    }
    return DefaultValue;
}

uint32 FRegistryKey::ReadDWord(const FString& ValueName, const uint32 DefaultValue) const
{
    const FRegistryValue Value = ReadValue(ValueName);
    if (Value.Type == ERegistryValueType::DWord)
    {
        return Value.AsDWord();
    }
    return DefaultValue;
}

uint64 FRegistryKey::ReadQWord(const FString& ValueName, const uint64 DefaultValue) const
{
    const FRegistryValue Value = ReadValue(ValueName);
    if (Value.Type == ERegistryValueType::QWord)
    {
        return Value.AsQWord();
    }
    return DefaultValue;
}

TArray<uint8> FRegistryKey::ReadBinary(const FString& ValueName) const
{
    const FRegistryValue Value = ReadValue(ValueName);
    if (Value.Type == ERegistryValueType::Binary)
    {
        return Value.AsBinary();
    }
    return {};
}

TArray<FString> FRegistryKey::ReadMultiString(const FString& ValueName) const
{
    const FRegistryValue Value = ReadValue(ValueName);
    if (Value.Type == ERegistryValueType::MultiString)
    {
        return Value.AsMultiString();
    }
    return {};
}

TArray<FString> FRegistryKey::GetSubKeyNames() const
{
    TArray<FString> SubKeys;
    if (!IsOpen)
    {
        return SubKeys;
    }
    FChar KeyName[256];
    DWORD Index = 0;
    while (true)
    {
        DWORD NameSize = 256;
        const LONG Result = RegEnumKeyEx(KeyHandle, Index++, KeyName, &NameSize, nullptr, nullptr, nullptr, nullptr);
        if (Result == ERROR_NO_MORE_ITEMS)
        {
            break;
        }
        if (Result == ERROR_SUCCESS)
        {
            SubKeys.PushBack(FString(KeyName));
        }
    }
    return SubKeys;
}

TArray<FString> FRegistryKey::GetValueNames() const
{
    TArray<FString> ValueNames;
    if (!IsOpen)
    {
        return ValueNames;
    }
    FChar ValueName[16384];
    DWORD Index = 0;
    while (true)
    {
        DWORD NameSize = 16384;
        const LONG Result = RegEnumValue(KeyHandle, Index++, ValueName, &NameSize, nullptr, nullptr, nullptr, nullptr);
        if (Result == ERROR_NO_MORE_ITEMS)
        {
            break;
        }
        if (Result == ERROR_SUCCESS)
        {
            ValueNames.PushBack(FString(ValueName));
        }
    }
    return ValueNames;
}

TArray<FRegistryValue> FRegistryKey::GetValues() const
{
    TArray<FRegistryValue> Values;
    const TArray<FString> ValueNames = GetValueNames();
    for (const auto& Name : ValueNames)
    {
        FRegistryValue Value = ReadValue(Name);
        if (Value.Type != ERegistryValueType::None)
        {
            Values.PushBack(std::move(Value));
        }
    }
    return Values;
}

FString FRegistryKey::GetFullPath() const
{
    return GetHiveName(Hive) + TEXT("\\") + SubKey;
}

HKEY FRegistryKey::GetHiveHandle() const
{
    return ::GetHiveHandle(Hive);
}

bool8 FRegistry::KeyExists(const ERegistryHive Hive, const FString& SubKey)
{
    const FRegistryKey Key(Hive, SubKey);
    return Key.Exists();
}

bool8 FRegistry::CreateKey(const ERegistryHive Hive, const FString& SubKey)
{
    FRegistryKey Key(Hive, SubKey);
    return Key.Create();
}

bool8 FRegistry::DeleteKey(const ERegistryHive Hive, const FString& SubKey)
{
    FRegistryKey Key(Hive, SubKey);
    return Key.Delete();
}

bool8 FRegistry::DeleteKeyTree(const ERegistryHive Hive, const FString& SubKey)
{
    const LONG Result = RegDeleteTree(GetHiveHandle(Hive), SubKey.c_str());
    return Result == ERROR_SUCCESS;
}

FString FRegistry::ReadString(const ERegistryHive Hive, const FString& SubKey, const FString& ValueName, const FString& DefaultValue)
{
    FRegistryKey Key(Hive, SubKey);
    if (Key.Open(true))
    {
        return Key.ReadString(ValueName, DefaultValue);
    }
    return DefaultValue;
}

uint32 FRegistry::ReadDWord(const ERegistryHive Hive, const FString& SubKey, const FString& ValueName, const uint32 DefaultValue)
{
    FRegistryKey Key(Hive, SubKey);
    if (Key.Open(true))
    {
        return Key.ReadDWord(ValueName, DefaultValue);
    }
    return DefaultValue;
}

uint64 FRegistry::ReadQWord(const ERegistryHive Hive, const FString& SubKey, const FString& ValueName, const uint64 DefaultValue)
{
    FRegistryKey Key(Hive, SubKey);
    if (Key.Open(true))
    {
        return Key.ReadQWord(ValueName, DefaultValue);
    }
    return DefaultValue;
}

bool8 FRegistry::WriteString(const ERegistryHive Hive, const FString& SubKey, const FString& ValueName, const FString& Value)
{
    FRegistryKey Key(Hive, SubKey);
    if (Key.Create() || Key.Open(false))
    {
        return Key.WriteString(ValueName, Value);
    }
    return false;
}

bool8 FRegistry::WriteDWord(const ERegistryHive Hive, const FString& SubKey, const FString& ValueName, const uint32 Value)
{
    FRegistryKey Key(Hive, SubKey);
    if (Key.Create() || Key.Open(false))
    {
        return Key.WriteDWord(ValueName, Value);
    }
    return false;
}

bool8 FRegistry::WriteQWord(const ERegistryHive Hive, const FString& SubKey, const FString& ValueName, const uint64 Value)
{
    FRegistryKey Key(Hive, SubKey);
    if (Key.Create() || Key.Open(false))
    {
        return Key.WriteQWord(ValueName, Value);
    }
    return false;
}
