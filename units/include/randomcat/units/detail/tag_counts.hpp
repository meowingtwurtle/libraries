#pragma once

#include <type_traits>

#include <randomcat/type_container/type_value_map.hpp>

#include "randomcat/units/detail/static_math.hpp"

namespace randomcat::units::detail {
    using tag_count = int16_t;

    using empty_tag_count_map = type_container::empty_type_value_map<tag_count>;

    template<typename M, int Multiple>
    struct tag_count_map_multiply {
        template<typename K, tag_count BeginValue>
        struct do_transform {
            static auto constexpr value = BeginValue * Multiple;
        };

        using type = type_container::type_value_map_transform_value_t<M, do_transform>;
    };
    
    template<typename M, int Multiple>
    using tag_count_map_multiply_t = typename tag_count_map_multiply<M, Multiple>::type;

    template<typename M1, typename M2>
    using tag_count_map_sum_t = type_container::type_value_map_merge_t<M1, M2, integral_sum<tag_count>::value_for, 0>;

    template<typename Key, auto Value>
    struct node_has_zero_value : std::bool_constant<Value == 0> {};
    
    template<typename M>
    using tag_map_remove_zero_tags_t = type_container::type_value_map_remove_if_t<M, node_has_zero_value>;

    template<typename Map0, typename Map1>
    inline auto constexpr tag_maps_are_equal_v =  type_container::type_value_maps_are_equal_v<detail::tag_map_remove_zero_tags_t<Map0>, detail::tag_map_remove_zero_tags_t<Map1>>;
}