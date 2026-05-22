#pragma once
#include <cstdint>
#include <utility>

// Standard types
using uint = unsigned int;
using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;
using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;

// Assert sizes of types
static_assert(sizeof(uint) >= 4, "Invalid size of uint");
static_assert(sizeof(uint8) == 1, "Invalid size of uint8");
static_assert(sizeof(uint16) == 2, "Invalid size of uint16");
static_assert(sizeof(uint32) == 4, "Invalid size of uint32");
static_assert(sizeof(uint64) == 8, "Invalid size of uint64");

namespace wne
{
    enum class ColorBlending : uint8
    {
        Solid = 0,
        Addition = 1,
        Substraction = 2,
        Total = 3
    };

    enum class LightState : uint8
    {
        Disabled = 0,
        Enabled = 1,
        Total = 2
    };

    template <typename E>
    constexpr auto val(E e) noexcept
    {
        return static_cast<std::underlying_type_t<E>>(e);
    }
}
