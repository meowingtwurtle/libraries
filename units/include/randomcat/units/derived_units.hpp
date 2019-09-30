#pragma once

#include "randomcat/units/detail/ratio_util.hpp"
#include "randomcat/units/detail/tag_counts.hpp"
#include "randomcat/units/detail/unit_util.hpp"
#include "randomcat/units/unit.hpp"

namespace randomcat::units {
    template<typename Tag>
    using simple_unit = unit<detail::empty_tag_count_map::set<Tag, 1>>;

    template<typename Unit, detail::tag_count Pow>
    using power_unit =
        unit<detail::tag_count_map_multiply_t<detail::unit_tag_counts_t<Unit>, Pow>, detail::ratio_pow_t<detail::unit_scale_t<Unit>, Pow>>;

    template<typename Unit>
    using inverse_unit = power_unit<Unit, -1>;

    template<typename Unit, typename Scale>
    using scale_unit = unit<detail::unit_tag_counts_t<Unit>, std::ratio_multiply<detail::unit_scale_t<Unit>, Scale>>;

    template<typename Unit1, typename Unit2>
    using product_unit = detail::product_unit_t<Unit1, Unit2>;

    template<typename Unit1, typename Unit2>
    using quotient_unit = product_unit<Unit1, inverse_unit<Unit2>>;

    using no_unit = unit<detail::empty_tag_count_map, std::ratio<1, 1>>;
}    // namespace randomcat::units