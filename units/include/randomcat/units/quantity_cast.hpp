#pragma once

#include <chrono>
#include <cstdint>
#include <ratio>
#include <type_traits>

#include "randomcat/units/detail/quantity_util.hpp"
#include "randomcat/units/detail/unit_util.hpp"
#include "randomcat/units/quantity_traits.hpp"
#include "randomcat/units/unit_traits.hpp"

namespace randomcat::units {
    template<typename ToQuantity, typename FromRep, typename FromUnit, typename = std::enable_if_t<unit_tags_are_equal_v<FromUnit, detail::quantity_unit_t<ToQuantity>>>>
    inline constexpr ToQuantity quantity_cast(quantity<FromRep, FromUnit> const& _value) {
        using ToUnit = detail::quantity_unit_t<ToQuantity>;
        using ToRep = detail::quantity_rep_t<ToQuantity>;

        using math_type = std::conditional_t<std::is_floating_point_v<ToRep> || std::is_floating_point_v<FromRep>, long double, std::intmax_t>;
        using scale_ratio = std::ratio_divide<detail::unit_scale_t<FromUnit>, detail::unit_scale_t<ToUnit>>;

        auto fromCount = static_cast<math_type>(_value.count());

        if constexpr (std::chrono::treat_as_floating_point_v<math_type>) {
            return ToQuantity{static_cast<ToRep>(fromCount / scale_ratio::den) * scale_ratio::num};
        } else {
            return ToQuantity{static_cast<ToRep>((fromCount * scale_ratio::num) / scale_ratio::den)};
        }
    }
}    // namespace randomcat::units