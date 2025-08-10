#pragma once

using int8 = signed char;
using int16 = short;
using int32 = int;
using int64 = long long;

using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long long;

using size64 = uint64;
using byte8 = char;

using float32 = float;
using float64 = double;

using bool8 = bool;

static_assert(sizeof(int8) == 1, "int8 must be 1 byte");
static_assert(sizeof(int16) == 2, "int16 must be 2 bytes");
static_assert(sizeof(int32) == 4, "int32 must be 4 bytes");
static_assert(sizeof(int64) == 8, "int64 must be 8 bytes");

static_assert(sizeof(uint8) == 1, "uint8 must be 1 byte");
static_assert(sizeof(uint16) == 2, "uint16 must be 2 bytes");
static_assert(sizeof(uint32) == 4, "uint32 must be 4 bytes");
static_assert(sizeof(uint64) == 8, "uint64 must be 8 bytes");

static_assert(sizeof(size64) == 8, "size64 must be 8 bytes");
static_assert(sizeof(byte8) == 1, "byte8 must be 1 byte");

static_assert(sizeof(float32) == 4, "float32 must be 4 bytes");
static_assert(sizeof(float64) == 8, "float64 must be 8 bytes");

static_assert(sizeof(bool8) == 1, "bool8 must be 1 byte");