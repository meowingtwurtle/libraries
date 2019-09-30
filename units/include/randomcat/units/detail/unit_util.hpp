#pragma once

#include <type_traits>

#include "randomcat/units/unit.hpp"

namespace randomcat::units::detail {
    template<typename T>
    struct is_unit : std::false_type {};

    template<typename... Ts>
    struct is_unit<unit<Ts...>> : std::true_type {};

    template<typename T>
    inline auto constexpr is_unit_v = is_unit<T>::value;

    template<typename T>
    struct unit_info {
        static_assert(invalid<T>, "Argument must be a unit");
    };

    template<typename TagCountMap, typename Scale>
    struct unit_info<unit<TagCountMap, Scale>> {
        using tag_counts = TagCountMap;
        using scale = Scale;
    };

    template<typename T>
    using unit_tag_counts_t = typename unit_info<T>::tag_counts;

    template<typename T>
    using unit_scale_t = typename unit_info<T>::scale;

    template<typename U1, typename U2>
    using product_unit_t =
        unit<tag_count_map_sum_t<unit_tag_counts_t<U1>, unit_tag_counts_t<U2>>, std::ratio_multiply<unit_scale_t<U1>, unit_scale_t<U2>>>;

    template<typename Unit, typename Scale>
    using unit_with_scale_t = unit<unit_tag_counts_t<Unit>, Scale>;

    template<typename Unit>
    using unit_without_scale_t = unit_with_scale_t<Unit, std::ratio<1, 1>>;

    template<typename U>
    inline auto constexpr unit_is_unitless_v =
        type_container::type_value_map_is_empty_v<unit_tag_counts_t<U>> || type_container::type_value_map_all_match_v<unit_tag_counts_t<U>, node_has_zero_value>;

    template<typename... Tags>
    using unit_from_tags_t = unit<type_container::type_list<Tags...>, std::ratio<1, 1>>;
}    // namespace randomcat::units::detail