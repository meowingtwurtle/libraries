#pragma once

#include <ratio>

#include "randomcat/units/detail/tag_counts.hpp"
#include "randomcat/units/detail/unit_util.hpp"

namespace randomcat::units {
    template<typename Unit1, typename Unit2>
    inline auto constexpr unit_tags_are_equal_v = detail::tag_maps_are_equal_v<detail::unit_tag_counts_t<Unit1>, detail::unit_tag_counts_t<Unit2>>;

    template<typename Unit1, typename Unit2>
    inline auto constexpr units_are_equal_v = unit_tags_are_equal_v<Unit1, Unit2> && std::ratio_equal_v<detail::unit_scale_t<Unit2>, detail::unit_scale_t<Unit2>>;

    template<typename Unit>
    inline auto constexpr unit_is_unitless_v = detail::unit_is_unitless_v<Unit>;
}    // namespace randomcat::units