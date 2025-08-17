#pragma once

#include <chrono>

#include "Engine/Core/CoreDefinitions.hpp"
#include "Engine/Core/CoreTypes.hpp"

template <typename TRep, typename TPeriod>
struct ENGINE_API TDuration
{
public:
    using FDurationType = std::chrono::duration<TRep, TPeriod>;
    using FRepType = TRep;
    using FPeriodType = TPeriod;

public:
    constexpr TDuration() noexcept = default;

    constexpr explicit TDuration(TRep Value) noexcept
        : Duration(Value)
    {
    }

    template <typename TOtherRep, typename TOtherPeriod>
    constexpr explicit TDuration(const std::chrono::duration<TOtherRep, TOtherPeriod>& InDuration) noexcept
        : Duration(std::chrono::duration_cast<FDurationType>(InDuration))
    {
    }

    template <typename TOtherRep, typename TOtherPeriod>
    constexpr explicit TDuration(const TDuration<TOtherRep, TOtherPeriod>& InDuration) noexcept
        : Duration(std::chrono::duration_cast<FDurationType>(InDuration.GetNative()))
    {
    }

    ~TDuration() noexcept = default;

    DEFAULT_COPY_MOVEABLE_PREFIX(TDuration, constexpr)

public:
    [[nodiscard]] constexpr TRep GetCount() const noexcept
    {
        return Duration.count();
    }

    [[nodiscard]] constexpr bool8 IsZero() const noexcept
    {
        return Duration.count() == TRep{};
    }

    [[nodiscard]] constexpr bool8 IsNegative() const noexcept
    {
        return Duration.count() < TRep{};
    }

    [[nodiscard]] constexpr bool8 IsPositive() const noexcept
    {
        return Duration.count() > TRep{};
    }

    [[nodiscard]] constexpr TDuration Absolute() const noexcept
    {
        if constexpr (std::is_signed_v<TRep>)
        {
            return TDuration(Duration.count() < TRep{} ? -Duration.count() : Duration.count());
        }
        else
        {
            return *this;
        }
    }

    [[nodiscard]] constexpr FDurationType GetNativeDuration() const noexcept { return Duration; }

public:
    template <typename T>
    [[nodiscard]] constexpr TDuration<std::common_type_t<TRep, typename T::FRepType>, std::ratio<1>> operator+(const T& Other) const noexcept requires std::is_base_of_v<
        TDuration<typename T::FRepType, typename T::FPeriodType>, T>
    {
        using FCommonDuration = std::chrono::duration<std::common_type_t<TRep, typename T::FRepType>>;
        return TDurationBase<std::common_type_t<TRep, typename T::RepType>, std::ratio<1>>(std::chrono::duration_cast<FCommonDuration>(Duration + Other.GetNativeDuration()));
    }

    template <typename T>
    [[nodiscard]] constexpr TDuration<std::common_type_t<TRep, typename T::FRepType>, std::ratio<1>> operator-(const T& Other) const noexcept requires std::is_base_of_v<
        TDuration<typename T::FRepType, typename T::FPeriodType>, T>
    {
        using CommonDuration = std::chrono::duration<std::common_type_t<TRep, typename T::FRepType>>;
        return TDuration<std::common_type_t<TRep, typename T::FRepType>, std::ratio<1>>(std::chrono::duration_cast<CommonDuration>(Duration - Other.GetNativeDuration()));
    }

    template <typename TScalar>
    [[nodiscard]] constexpr TDuration operator*(TScalar Scalar) const noexcept
        requires std::is_arithmetic_v<TScalar>
    {
        if constexpr (std::is_floating_point_v<TRep> || std::is_floating_point_v<TScalar>)
        {
            return TDuration<float64, TPeriod>(Duration.count() * static_cast<float64>(Scalar));
        }
        else
        {
            return TDuration(static_cast<TRep>(Duration.count() * Scalar));
        }
    }

    template <typename TScalar>
    [[nodiscard]] constexpr TDuration operator/(TScalar Scalar) const noexcept
        requires std::is_arithmetic_v<TScalar>
    {
        if constexpr (std::is_floating_point_v<TRep> || std::is_floating_point_v<TScalar>)
        {
            return TDuration<float64, TPeriod>(Duration.count() / static_cast<float64>(Scalar));
        }
        else
        {
            return TDuration(static_cast<TRep>(Duration.count() / Scalar));
        }
    }

    template <typename T>
    constexpr TDuration& operator+=(const T& Other) noexcept
        requires std::is_base_of_v<TDuration<typename T::FRepType, typename T::FPeriodType>, T>
    {
        Duration += std::chrono::duration_cast<FDurationType>(Other.GetNativeDuration());
        return static_cast<auto&>(*this);
    }

    template <typename T>
    constexpr TDuration& operator-=(const T& Other) noexcept
        requires std::is_base_of_v<TDuration<typename T::FRepType, typename T::FPeriodType>, T>
    {
        Duration -= std::chrono::duration_cast<FDurationType>(Other.GetNativeDuration());
        return static_cast<auto&>(*this);
    }

    template <typename TScalar>
    constexpr TDuration& operator*=(TScalar Scalar) noexcept
        requires std::is_arithmetic_v<TScalar>
    {
        if constexpr (std::is_floating_point_v<TRep>)
        {
            Duration = FDurationType(Duration.count() * static_cast<TRep>(Scalar));
        }
        else
        {
            Duration = FDurationType(static_cast<TRep>(Duration.count() * Scalar));
        }
        return static_cast<auto&>(*this);
    }

    template <typename TScalar>
    constexpr TDuration& operator/=(TScalar Scalar) noexcept
        requires std::is_arithmetic_v<TScalar>
    {
        if constexpr (std::is_floating_point_v<TRep>)
        {
            Duration = FDurationType(Duration.count() / static_cast<TRep>(Scalar));
        }
        else
        {
            Duration = FDurationType(static_cast<TRep>(Duration.count() / Scalar));
        }
        return static_cast<auto&>(*this);
    }

    template <typename T>
    [[nodiscard]] constexpr bool8 operator<=>(const T& Other) const noexcept
        requires std::is_base_of_v<TDuration<typename T::FRepType, typename T::FPeriodType>, T>
    {
        return Duration <=> Other.GetNativeDuration();
    }

    template <typename T>
    [[nodiscard]] constexpr bool8 operator==(const T& Other) const noexcept
        requires std::is_base_of_v<TDuration<typename T::FRepType, typename T::FPeriodType>, T>
    {
        return Duration == Other.GetNativeDuration();
    }

public:
    [[nodiscard]] static constexpr TDuration Zero() noexcept
    {
        return TDuration(TRep{});
    }

    [[nodiscard]] static constexpr TDuration Min() noexcept
    {
        return TDuration(FDurationType::min());
    }

    [[nodiscard]] static constexpr TDuration Max() noexcept
    {
        return TDuration(FDurationType::max());
    }

protected:
    FDurationType Duration = {};
};

using FNanoSeconds = TDuration<int64, std::nano>;
using FMicroSeconds = TDuration<int64, std::micro>;
using FMilliSeconds = TDuration<int64, std::milli>;
using FSeconds = TDuration<float64, std::ratio<1>>;
using FMinutes = TDuration<float64, std::ratio<60>>;
using FHours = TDuration<float64, std::ratio<3600>>;

template <typename T>
concept CDuration = requires
{
    typename T::FRepType;
    typename T::FPeriodType;
    typename T::FDurationType;
} && std::is_base_of_v<TDuration<typename T::FRepType, typename T::FPeriodType>, T>;

class ENGINE_API FTimePoint final
{
public:
    using FClockType = std::chrono::high_resolution_clock;
    using FTimePointType = FClockType::time_point;

public:
    constexpr FTimePoint() noexcept = default;

    constexpr explicit FTimePoint(const FTimePointType InTimePoint) noexcept
        : TimePoint(InTimePoint)
    {
    }

    constexpr ~FTimePoint() noexcept = default;

    DEFAULT_COPY_MOVEABLE_PREFIX(FTimePoint, constexpr)

public:
    [[nodiscard]] constexpr FNanoSeconds GetNanoSeconds() const noexcept
    {
        return FNanoSeconds(std::chrono::duration_cast<std::chrono::nanoseconds>(TimePoint.time_since_epoch()));
    }

    [[nodiscard]] constexpr FMicroSeconds GetMicroSeconds() const noexcept
    {
        return FMicroSeconds(std::chrono::duration_cast<std::chrono::microseconds>(TimePoint.time_since_epoch()));
    }

    [[nodiscard]] constexpr FMilliSeconds GetMilliSeconds() const noexcept
    {
        return FMilliSeconds(std::chrono::duration_cast<std::chrono::milliseconds>(TimePoint.time_since_epoch()));
    }

    [[nodiscard]] constexpr FSeconds GetSeconds() const noexcept
    {
        return FSeconds(std::chrono::duration<float64>(TimePoint.time_since_epoch()));
    }

    [[nodiscard]] constexpr FMinutes GetMinutes() const noexcept
    {
        return FMinutes(std::chrono::duration<float64, std::ratio<60>>(TimePoint.time_since_epoch()));
    }

    [[nodiscard]] constexpr FHours GetHours() const noexcept
    {
        return FHours(std::chrono::duration<float64, std::ratio<3600>>(TimePoint.time_since_epoch()));
    }

    [[nodiscard]] constexpr int64 GetTicks() const noexcept
    {
        return TimePoint.time_since_epoch().count();
    }

    [[nodiscard]] constexpr const FTimePointType& GetNativeTimePoint() const noexcept
    {
        return TimePoint;
    }

public:
    template <CDuration TDuration>
    [[nodiscard]] constexpr FTimePoint operator+(const TDuration& InDuration) const noexcept
    {
        return FTimePoint(TimePoint + InDuration.GetNativeDuration());
    }

    template <CDuration TDuration>
    [[nodiscard]] constexpr FTimePoint operator-(const TDuration& InDuration) const noexcept
    {
        return FTimePoint(TimePoint - InDuration.GetNativeDuration());
    }

    template <CDuration TDuration>
    FTimePoint& operator+=(const TDuration& InDuration) noexcept
    {
        TimePoint += InDuration.GetNativeDuration();
        return *this;
    }

    template <CDuration TDuration>
    FTimePoint& operator-=(const TDuration& InDuration) noexcept
    {
        TimePoint -= InDuration.GetNativeDuration();
        return *this;
    }

public:
    [[nodiscard]] constexpr auto operator<=>(const FTimePoint& Other) const noexcept = default;
    [[nodiscard]] constexpr bool8 operator==(const FTimePoint& Other) const noexcept = default;

public:
    [[nodiscard]] static FTimePoint Now() noexcept
    {
        return FTimePoint(FClockType::now());
    }

    template <CDuration TDuration>
    [[nodiscard]] static constexpr FTimePoint FromDuration(const TDuration& Duration) noexcept
    {
        return FTimePoint(FTimePointType(Duration.GetNativeDuration()));
    }

private:
    FTimePointType TimePoint = {};
};
