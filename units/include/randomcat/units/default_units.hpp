#pragma once

#include <randomcat/units/units.hpp>

namespace randomcat::units {
    inline namespace default_units {
        namespace default_units_detail {
            using pi_ratio = std::ratio<1068966896, 340262731>;
        }

#define RC_INT_UNIT_REP int64_t
#define RC_FRAC_UNIT_REP double

#define RC_UNIT_DEF_REP(rep, unit_name, ...) using unit_name##_unit = __VA_ARGS__; using unit_name = quantity<rep, unit_name##_unit>;
#define RC_INT_UNIT_DEF(unit_name, ...) RC_UNIT_DEF_REP(RC_INT_UNIT_REP, unit_name, __VA_ARGS__);
#define RC_FRAC_UNIT_DEF(unit_name, ...) RC_UNIT_DEF_REP(RC_FRAC_UNIT_REP, unit_name, __VA_ARGS__);
#define RC_BASE_UNIT_DEF_REP(define_macro, unit_name, tag_name) define_macro(unit_name, simple_unit<::randomcat::units::detail::default_tags::tag_name##_tag>);
#define RC_INT_BASE_UNIT_DEF(unit_name, tag_name) RC_BASE_UNIT_DEF_REP(RC_INT_UNIT_DEF, unit_name, tag_name);
#define RC_FRAC_BASE_UNIT_DEF(unit_name, tag_name) RC_BASE_UNIT_DEF_REP(RC_FRAC_UNIT_DEF, unit_name, tag_name);

        RC_INT_BASE_UNIT_DEF (seconds, time);
        RC_INT_BASE_UNIT_DEF (amperes, current);
        RC_INT_BASE_UNIT_DEF (kelvins, temperature);
        RC_INT_BASE_UNIT_DEF (meters, distance);
        RC_INT_BASE_UNIT_DEF (kilograms, mass);
        RC_INT_BASE_UNIT_DEF (candelas, luminous_intensity);
        RC_FRAC_BASE_UNIT_DEF(radians, angle);

        RC_INT_UNIT_DEF(newtons, product_unit<kilograms_unit, quotient_unit<meters_unit, power_unit<seconds_unit, 2>>>);
        RC_INT_UNIT_DEF(joules, product_unit<newtons_unit, meters_unit>);
        RC_INT_UNIT_DEF(watts, quotient_unit<joules_unit, seconds_unit>);
        RC_INT_UNIT_DEF(volts, quotient_unit<watts_unit, amperes_unit>);
        RC_INT_UNIT_DEF(ohms, quotient_unit<volts_unit, amperes_unit>);

        RC_INT_UNIT_DEF(inches, scale_unit<meters_unit, std::ratio<254, 10000>>);
        RC_INT_UNIT_DEF(feet, scale_unit<inches_unit, std::ratio<12, 1>>);
        RC_INT_UNIT_DEF(yards, scale_unit<feet_unit, std::ratio<3, 1>>);
        RC_INT_UNIT_DEF(miles, scale_unit<feet_unit, std::ratio<5280, 1>>);

        RC_FRAC_UNIT_DEF(degrees, scale_unit<radians_unit, std::ratio_divide<default_units_detail::pi_ratio, std::ratio<180, 1>>>);

#define RC_PREFIX_UNIT_DEF(base_unit, prefix, ...) RC_INT_UNIT_DEF(prefix##base_unit, scale_unit<base_unit##_unit, __VA_ARGS__>);
#define RC_PREFIX_UNITS_DEF(base_unit) RC_PREFIX_UNIT_DEF(base_unit, milli, std::milli); RC_PREFIX_UNIT_DEF(base_unit, micro, std::micro); RC_PREFIX_UNIT_DEF(base_unit, kilo, std::kilo)

        RC_PREFIX_UNITS_DEF(seconds);
        RC_PREFIX_UNITS_DEF(amperes);
        RC_PREFIX_UNITS_DEF(kelvins);
        RC_PREFIX_UNITS_DEF(meters);

        using grams_unit = scale_unit<kilograms_unit, std::ratio<1, 1000>>;
        RC_PREFIX_UNIT_DEF(grams, milli, std::milli);
        RC_PREFIX_UNIT_DEF(grams, micro, std::micro);

        RC_PREFIX_UNITS_DEF(candelas);
        RC_PREFIX_UNITS_DEF(newtons);
        RC_PREFIX_UNITS_DEF(joules);
        RC_PREFIX_UNITS_DEF(watts);
        RC_PREFIX_UNITS_DEF(volts);
        RC_PREFIX_UNITS_DEF(ohms);

#undef RC_PREFIX_UNITS_DEF
#undef RC_PREFIX_UNIT_DEF
#undef RC_INT_BASE_UNIT_DEF
#undef RC_INT_UNIT_DEF
#undef RC_FRAC_UNIT_REP
#undef RC_INT_UNIT_REP
        
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
