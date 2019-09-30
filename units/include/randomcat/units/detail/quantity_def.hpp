#pragma once

#include <type_traits>

#include "randomcat/units/detail/default_tags.hpp"
#include "randomcat/units/detail/quantity_fwd.hpp"
#include "randomcat/units/detail/tag_counts.hpp"
#include "randomcat/units/detail/unit_util.hpp"
#include "randomcat/units/quantity_cast.hpp"
#include "randomcat/units/derived_units.hpp"

namespace randomcat::units {
    namespace detail {
        template<typename Derived, typename Enable = void>
        class chrono_quantity_base {
        public:
            inline static constexpr auto is_time = false;
            using chrono_type = void;
        };

        using time_tag_map = detail::empty_tag_count_map::set<detail::default_tags::time_tag, 1>;

        template<typename Rep, typename Unit>
        class chrono_quantity_base<quantity<Rep, Unit>, std::enable_if_t<detail::tag_maps_are_equal_v<detail::unit_tag_counts_t<Unit>, time_tag_map>>> {
        private:
            constexpr auto derived_this() const noexcept { return static_cast<quantity<Rep, Unit> const*>(this); }

        public:
            using chrono_type = std::chrono::duration<Rep, detail::unit_scale_t<Unit>>;
            inline static constexpr auto is_time = true;

            constexpr chrono_type as_chrono() const noexcept(noexcept(chrono_type(derived_this()->count()))) {
                return chrono_type(derived_this()->count());
            }

            constexpr operator chrono_type() const noexcept(noexcept(as_chrono())) { return as_chrono(); }
        };

        template<typename Derived, typename Enable = void>
        class unitless_quantity_base {};

        template<typename Rep, typename Unit>
        class unitless_quantity_base<quantity<Rep, Unit>, std::enable_if_t<unit_is_unitless_v<Unit>>> {
        private:
            using derived = quantity<Rep, Unit>;
            using scale = unit_scale_t<Unit>;

            constexpr auto derived_this() const noexcept { return static_cast<derived const*>(this); }

        public:
            constexpr operator auto() const noexcept(noexcept(derived_this()->count())) {
                static_assert(std::chrono::treat_as_floating_point_v<Rep> || (scale::den == 1), "Possible loss of precision in conversion.");

                return derived_this()->count() * scale::num / scale::den;
            }
        };
    }    // namespace detail

    template<typename Rep, typename Unit>
    class quantity : public detail::chrono_quantity_base<quantity<Rep, Unit>>, public detail::unitless_quantity_base<quantity<Rep, Unit>> {
        static_assert(detail::is_unit_v<Unit>, "Unit must be a unit");
        static_assert(std::is_same_v<Rep, std::remove_cv_t<std::remove_reference_t<Rep>>>, "Rep cannot be cv qualified or a reference");
        static_assert(!is_quantity_v<Rep>, "Rep cannot be a quantity");

    private:
        using chrono_base = detail::chrono_quantity_base<quantity<Rep, Unit>>;
        inline static constexpr auto is_time = chrono_base::is_time;
        using chrono_type = typename chrono_base::chrono_type;

    public:
        using rep = Rep;
        using unit = Unit;

        static_assert(std::is_nothrow_copy_constructible_v<Rep>);
        static_assert(std::is_nothrow_move_constructible_v<Rep>);
        static_assert(noexcept(std::declval<Rep>() + std::declval<Rep>()));
        static_assert(noexcept(std::declval<Rep>() - std::declval<Rep>()));
        static_assert(noexcept(std::declval<Rep>() * std::declval<Rep>()));
        static_assert(noexcept(std::declval<Rep>() / std::declval<Rep>()));
        static_assert(noexcept(std::declval<std::conditional_t<!std::is_floating_point_v<Rep>, Rep, int>>() % std::declval<int>()));
        static_assert(noexcept(-std::declval<Rep>()));

        constexpr explicit quantity(Rep _value) noexcept : m_value(std::move(_value)) {}

        template<typename Rep2,
                 typename Unit2,
                 typename = std::enable_if_t<unit_tags_are_equal_v<Unit, Unit2> && (std::chrono::treat_as_floating_point_v<Rep> || (std::ratio_divide<detail::unit_scale_t<Unit2>, detail::unit_scale_t<Unit>>::den == 1))>>
        constexpr quantity(quantity<Rep2, Unit2> const& _other) noexcept : m_value{units::quantity_cast<quantity>(_other).count()} {}

        template<bool Enable = is_time>
        /* implicit */ constexpr quantity(std::enable_if_t<Enable, chrono_type> const& _asChrono) noexcept : m_value{_asChrono.count()} {}

        constexpr auto count() const noexcept { return m_value; }

        template<typename OtherUnit, typename = std::enable_if_t<units_are_equal_v<Unit, OtherUnit>>>
            constexpr quantity& operator+=(quantity<Rep, OtherUnit> const& _other) & noexcept(noexcept(m_value += _other.count())) {
            m_value += _other.count();
            return *this;
        }

        template<typename OtherRep, typename OtherUnit, typename = std::enable_if_t<units_are_equal_v<Unit, OtherUnit>>>
            constexpr quantity& operator-=(quantity<OtherRep, OtherUnit> const& _other) & noexcept(noexcept(m_value -= _other.count())) {
            m_value -= _other.count();
            return *this;
        }

        template<typename OtherRep, typename = std::enable_if_t<std::is_arithmetic_v<OtherRep>>>
            constexpr quantity& operator*=(OtherRep const& _other) & noexcept(noexcept(m_value *= _other)) {
            m_value *= _other;
            return *this;
        }

        template<typename OtherRep, typename R = Rep, typename = std::enable_if_t<std::is_arithmetic_v<OtherRep> && std::is_floating_point_v<R>>>
            constexpr quantity& operator/=(OtherRep const& _other) & noexcept(noexcept(m_value /= _other)) {
            m_value /= _other;
            return *this;
        }

        template<typename OtherRep, typename = std::enable_if_t<std::is_integral_v<OtherRep>>>
            constexpr quantity& operator%=(OtherRep const& _other) & noexcept(noexcept(m_value %= _other)) {
            m_value %= _other;
            return *this;
        }

        constexpr quantity operator+() const noexcept { return *this; }

        constexpr auto operator-() const noexcept(noexcept(-m_value)) {
            using result_rep = std::decay_t<decltype(-m_value)>;

            return quantity<result_rep, Unit>{-m_value};
        }

    private:
        Rep m_value;
    };

    template<typename Unit, typename Rep>
    constexpr auto make_quantity(Rep&& _value) noexcept(noexcept(quantity<std::decay_t<Rep>, Unit>(std::forward<Rep>(_value)))) {
        return quantity<std::decay_t<Rep>, Unit>(std::forward<Rep>(_value));
    }

    template<typename Rep>
    constexpr auto unitless_quantity(Rep&& _value) noexcept(noexcept(units::make_quantity<no_unit>(std::forward<Rep>(_value)))) {
        return units::make_quantity<no_unit>(std::forward<Rep>(_value));
    }
}    // namespace randomcat::units

namespace std {
    template<typename Rep1, typename Unit1, typename Rep2, typename Unit2>
    struct common_type<randomcat::units::quantity<Rep1, Unit1>, randomcat::units::quantity<Rep2, Unit2>> {
    private:
        using scale1 = randomcat::units::detail::unit_scale_t<Unit1>;
        using scale2 = randomcat::units::detail::unit_scale_t<Unit2>;

        static_assert(randomcat::units::unit_tags_are_equal_v<Unit1, Unit2>, "Units must have the same tags");

    public:
        using type =
            randomcat::units::quantity<std::common_type_t<Rep1, Rep2>,
                                       randomcat::units::unit<randomcat::units::detail::unit_tag_counts_t<Unit1>, randomcat::units::detail::ratio_gcd_t<scale1, scale2>>>;
    };
}    // namespace std