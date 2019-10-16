#pragma once

#include <algorithm>
#include <cmath>
#include <type_traits>

#include "randomcat/units/detail/quantity_def.hpp"

namespace randomcat::units {
    template<typename Rep, typename Unit>
    constexpr quantity<Rep, Unit> abs(quantity<Rep, Unit> const& _q) {
        using std::abs;
        return quantity<Rep, Unit>(abs(_q.count()));
    }

    template<std::intmax_t Power, typename Rep, typename Unit>
    constexpr auto pow(quantity<Rep, Unit> const& _quantity) {
        using std::pow;
        return units::make_quantity<power_unit<Unit, Power>>(pow(_quantity.count(), Power));
    }

    template<typename Rep1, typename Unit1, typename Rep2, typename Unit2, typename = std::enable_if_t<unit_tags_are_equal_v<Unit1, Unit2>>, typename common = std::common_type_t<quantity<Rep1, Unit1>, quantity<Rep2, Unit2>>>
    constexpr common operator+(quantity<Rep1, Unit1> const& _first, quantity<Rep2, Unit2> const& _second) noexcept(noexcept(common(common(_first).count() + common(_second).count()))) {
        return common(common(_first).count() + common(_second).count());
    }

    template<typename Rep1, typename Unit1, typename Rep2, typename Unit2>
    constexpr auto operator*(quantity<Rep1, Unit1> const& _first, quantity<Rep2, Unit2> const& _second) noexcept(noexcept(quantity<std::common_type_t<Rep1, Rep2>, product_unit<Unit1, Unit2>>(_first.count() * _second.count()))) {
        return quantity<std::common_type_t<Rep1, Rep2>, product_unit<Unit1, Unit2>>(_first.count() * _second.count());
    }

    template<typename ValueRep, typename QuantityRep, typename Unit, typename = std::enable_if_t<!is_quantity_v<ValueRep>>>
    constexpr auto operator*(ValueRep const& _val, quantity<QuantityRep, Unit> const& _quantity) noexcept(noexcept(units::unitless_quantity(_val) * _quantity)) {
        return units::unitless_quantity(_val) * _quantity;
    }

    template<typename ValueRep, typename QuantityRep, typename Unit, typename = std::enable_if_t<!is_quantity_v<ValueRep>>>
    constexpr auto operator*(quantity<QuantityRep, Unit> const& _quantity, ValueRep const& _val) noexcept(noexcept(_quantity * units::unitless_quantity(_val))) {
        return _quantity * units::unitless_quantity(_val);
    }

    template<typename Rep1, typename Unit1, typename Rep2, typename Unit2, typename common = std::common_type_t<quantity<Rep1, Unit1>, quantity<Rep2, Unit2>>>
    constexpr common operator-(quantity<Rep1, Unit1> const& _first, quantity<Rep2, Unit2> const& _second) noexcept(noexcept(common(_common(_first).count() - common(_second).count()))) {
        return common(_common(_first).count() - common(_second).count());
    }

    template<typename Rep1, typename Unit1, typename Rep2, typename Unit2>
    constexpr auto operator/(quantity<Rep1, Unit1> const& _first, quantity<Rep2, Unit2> const& _second) {
        using common_rep = std::common_type_t<Rep1, Rep2>;
        using result = quantity<common_rep, quotient_unit<detail::unit_without_scale_t<Unit1>, detail::unit_without_scale_t<Unit2>>>;

        using scale_gcd = detail::ratio_gcd_t<detail::unit_scale_t<Unit1>, detail::unit_scale_t<Unit2>>;
        using first_scaled = quantity<common_rep, unit<detail::unit_tag_counts_t<Unit1>, scale_gcd>>;
        using second_scaled = quantity<common_rep, unit<detail::unit_tag_counts_t<Unit2>, scale_gcd>>;

        return quantity<common_rep, quotient_unit<Unit1, Unit2>>{common_rep(common_rep(first_scaled(_first).count()) / common_rep(second_scaled{_second}.count())};
    }

    template<typename ValueRep, typename QuantityRep, typename QuantityUnit, typename = std::enable_if_t<!is_quantity_v<ValueRep>>>
    constexpr auto operator/(quantity<QuantityRep, QuantityUnit> const& _q, ValueRep const& _v) noexcept(noexcept(_q / units::unitless_quantity(_v))) {
        return _q / units::unitless_quantity(_v);
    }
}    // namespace randomcat::units