#pragma once

#include <cstdint>

#include "randomcat/units/quantity.hpp"
#include "randomcat/units/quantity_traits.hpp"
#include "randomcat/units/detail/default_tags.hpp"

namespace randomcat::units {
    namespace detail {
        using pi_ratio = std::ratio<1068966896, 340262731>;

        using default_integer_rep = std::int64_t;
        using default_fractional_rep = double;

        template<typename Quantity, typename Scale>
        using scale_quantity = quantity<quantity_rep_t<Quantity>, scale_unit<quantity_unit_t<Quantity>, Scale>>;
    }
    
    inline namespace default_units {
        using seconds_unit = simple_unit<detail::default_tags::time_tag>;
        using seconds = quantity<detail::default_integer_rep, seconds_unit>;

        using amperes_unit = simple_unit<detail::default_tags::current_tag>;
        using amperes = quantity<detail::default_integer_rep, amperes_unit>;

        using kelvins_unit = simple_unit<detail::default_tags::temperature_tag>;
        using kelvins = quantity<detail::default_integer_rep, kelvins_unit>;

        using meters_unit = simple_unit<detail::default_tags::distance_tag>;
        using meters = quantity<detail::default_integer_rep, meters_unit>;

        using kilograms_unit = simple_unit<detail::default_tags::mass_tag>;
        using kilograms = quantity<detail::default_integer_rep, kilograms_unit>;

        using candelas_unit = simple_unit<detail::default_tags::luminous_intensity_tag>;
        using candelas = quantity<detail::default_integer_rep, candelas_unit>;

        using radians_unit = simple_unit<detail::default_tags::angle_tag>;
        using radians = quantity<detail::default_fractional_rep, radians_unit>;

        using newtons_unit = product_unit<kilograms_unit, quotient_unit<meters_unit, power_unit<seconds_unit, 2>>>;
        using newtons = quantity<detail::default_integer_rep, newtons_unit>;

        using joules_unit = product_unit<newtons_unit, meters_unit>;
        using joules = quantity<detail::default_integer_rep, joules_unit>;

        using watts_unit = quotient_unit<joules_unit, seconds_unit>;
        using watts = quantity<detail::default_integer_rep, watts_unit>;

        using volts_unit = quotient_unit<watts_unit, amperes_unit>;
        using volts = quantity<detail::default_integer_rep, volts_unit>;

        using ohms_unit = quotient_unit<volts_unit, amperes_unit>;
        using ohms = quantity<detail::default_integer_rep, ohms_unit>;

        using inches_unit = scale_unit<meters_unit, std::ratio<254, 10000>>;
        using inches = quantity<detail::default_integer_rep, inches_unit>;

        using feet_unit = scale_unit<inches_unit, std::ratio<12, 1>>;
        using feet = quantity<detail::default_integer_rep, feet_unit>;

        using yards_unit = scale_unit<feet_unit, std::ratio<3, 1>>;
        using yards = quantity<detail::default_integer_rep, yards_unit>;

        using miles_unit = scale_unit<feet_unit, std::ratio<5280, 1>>;
        using miles = quantity<detail::default_integer_rep, miles_unit>;

        using degrees_unit = scale_unit<radians_unit, std::ratio_divide<detail::pi_ratio, std::ratio<180, 1>>>;
        using degrees = quantity<detail::default_fractional_rep, degrees_unit>;

#define RC_PREFIX_UNIT_DEF(base_unit, prefix, ratio)                                                                                       \
    using prefix##base_unit##_unit = scale_unit<base_unit##_unit, ratio>;                                                                  \
    using prefix##base_unit = detail::scale_quantity<base_unit, ratio>;

#define RC_PREFIX_UNITS_DEF(base_unit)                                                                                                     \
    RC_PREFIX_UNIT_DEF(base_unit, milli, std::milli);                                                                                      \
    RC_PREFIX_UNIT_DEF(base_unit, micro, std::micro);                                                                                      \
    RC_PREFIX_UNIT_DEF(base_unit, kilo, std::kilo);

        RC_PREFIX_UNITS_DEF(seconds);
        RC_PREFIX_UNITS_DEF(amperes);
        RC_PREFIX_UNITS_DEF(kelvins);
        RC_PREFIX_UNITS_DEF(meters);
        RC_PREFIX_UNITS_DEF(candelas);
        RC_PREFIX_UNITS_DEF(newtons);
        RC_PREFIX_UNITS_DEF(joules);
        RC_PREFIX_UNITS_DEF(watts);
        RC_PREFIX_UNITS_DEF(volts);
        RC_PREFIX_UNITS_DEF(ohms);

#undef RC_PREFIX_UNITS_DEF
#undef RC_PREFIX_UNIT_DEF

        using grams_unit = scale_unit<kilograms_unit, std::ratio<1, 1000>>;
        using grams = detail::scale_quantity<kilograms, std::ratio<1, 1000>>;

        using milligrams_unit = scale_unit<grams_unit, std::milli>;
        using milligrams = detail::scale_quantity<grams, std::milli>;

        using micrograms_unit = scale_unit<grams_unit, std::micro>;
        using micrograms = detail::scale_quantity<grams, std::micro>;
        
        inline auto sin(radians _angle) noexcept {
            return std::sin(_angle.count());
        }
        
        inline auto cos(radians _angle) noexcept {
            return std::cos(_angle.count());
        }
        
        inline auto tan(radians _angle) noexcept {
            return std::tan(_angle.count());
        }
        
        inline radians asin(double _value) noexcept {
            return radians(std::asin(_value));
        }
        
        inline radians acos(double _value) noexcept {
            return radians(std::acos(_value));
        }
        
        inline radians atan(double _v) noexcept {
            return radians(std::atan(_v));
        }
        
        inline radians atan(double _y, double _x) noexcept {
            return radians(std::atan2(_y, _x));
        }
        
        inline radians atan2(double _y, double _x) noexcept {
            return atan(_y, _x);
        }
    }


    // Ohm's law
    static_assert(default_units::amperes(2) * default_units::ohms(3) == default_units::volts(6));
    static_assert(default_units::milliamperes(3) * default_units::milliohms(2) == default_units::microvolts(6));

    // std::chrono compatibility
    static_assert(default_units::seconds(2) == default_units::milliseconds(2000));
    static_assert(std::chrono::seconds(2) == std::chrono::milliseconds(2000));

    static_assert(default_units::milliseconds(2995) == default_units::microseconds(2995000));
    static_assert(std::chrono::milliseconds(2995) == std::chrono::microseconds(2995000));

    static_assert(default_units::seconds(3) - default_units::milliseconds(5) == default_units::milliseconds(2995));
    static_assert(std::chrono::seconds(3) - std::chrono::milliseconds(5) == std::chrono::milliseconds(2995));

    static_assert(default_units::seconds(3) - default_units::seconds(5) == default_units::milliseconds(500) * -2 - default_units::seconds(1));
    static_assert(std::chrono::seconds(3) - std::chrono::seconds(5) == std::chrono::milliseconds(500) * -2 - std::chrono::seconds(1));

    static_assert(default_units::seconds(5) / default_units::seconds(2) == 2);
    static_assert(std::chrono::seconds(5) / std::chrono::seconds(2) == 2);

    static_assert(default_units::seconds(5) / 2 == default_units::seconds(2));
    static_assert(std::chrono::seconds(5) / 2 == std::chrono::seconds(2));

    static_assert(default_units::seconds(3) / default_units::milliseconds(2) == 1500);
    static_assert(std::chrono::seconds(3) / std::chrono::milliseconds(2) == 1500);

    static_assert(default_units::milliseconds(4) / default_units::seconds(2) == 0);
    static_assert(std::chrono::milliseconds(4) / std::chrono::seconds(2) == 0);

    static_assert(default_units::milliseconds(12) != default_units::microseconds(23));
    static_assert(std::chrono::milliseconds(12) != std::chrono::microseconds(23));

    static_assert(default_units::milliseconds(4) <= default_units::microseconds(4000));
    static_assert(std::chrono::milliseconds(4) <= std::chrono::microseconds(4000));

    static_assert(default_units::milliseconds(1) > default_units::microseconds(998));
    static_assert(std::chrono::milliseconds(1) > std::chrono::microseconds(998));

    static_assert(default_units::milliseconds(42).as_chrono() == std::chrono::microseconds(42000));

    static_assert(default_units::feet(5280 * 2) == default_units::miles(2));

    // Cannot static_assert abs/min/max because not constexpr in standard c++

    static_assert(make_quantity<default_units::seconds_unit>(5) == default_units::seconds(5));
    static_assert(make_quantity<default_units::seconds_unit>(0.00765) == default_units::microseconds(7650));
}
