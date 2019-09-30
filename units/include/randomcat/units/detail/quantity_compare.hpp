#pragma once

#include "randomcat/units/detail/quantity_def.hpp"

namespace randomcat::units {
    namespace detail {
        template<typename Rep1, typename Unit1, typename Rep2, typename Unit2, typename = std::enable_if_t<unit_tags_are_equal_v<Unit1, Unit2>>, typename Compare>
        constexpr auto with_common_counts(quantity<Rep1, Unit1> const& _first, quantity<Rep2, Unit2> const& _second, Compare _comp) noexcept {
            using common_quantity = std::common_type_t<quantity<Rep1, Unit1>, quantity<Rep2, Unit2>>;
            using common_rep = quantity_rep_t<common_quantity>;

            static_assert(std::is_convertible_v<std::invoke_result_t<Compare, common_rep, common_rep>, bool>);

            return _comp(common_quantity{_first}.count(), common_quantity{_second}.count());
        }
    }    // namespace detail

#define RC_QUANTITY_COMPARE_OP(OP)                                                                                                         \
                                                                                                                                           \
    template<typename Rep1, typename Unit1, typename Rep2, typename Unit2>                                                                 \
    constexpr auto operator OP(quantity<Rep1, Unit1> const& _first, quantity<Rep2, Unit2> const& _second) noexcept {                       \
        return detail::with_common_counts(                                                                                                 \
            _first, _second, [](auto x, auto y) constexpr noexcept { return x OP y; });                                                    \
    }

    RC_QUANTITY_COMPARE_OP(==);
    RC_QUANTITY_COMPARE_OP(!=);
    RC_QUANTITY_COMPARE_OP(<);
    RC_QUANTITY_COMPARE_OP(<=);
    RC_QUANTITY_COMPARE_OP(>);
    RC_QUANTITY_COMPARE_OP(>=);

#undef RC_QUANTITY_COMPARE_OP

    template<typename Rep1, typename Unit1, typename Rep2, typename Unit2, typename = std::enable_if_t<unit_tags_are_equal_v<Unit1, Unit2>>>
    constexpr auto min(quantity<Rep1, Unit1> const& _first, quantity<Rep2, Unit2> const& _second) {
        using common_quantity = std::common_type_t<quantity<Rep1, Unit1>, quantity<Rep2, Unit2>>;

        using std::min;
        return make_quantity<Unit1>(min(common_quantity{_first}.count(), common_quantity{_second}.count()));
    }

    template<typename Rep1, typename Unit1, typename Rep2, typename Unit2, typename = std::enable_if_t<unit_tags_are_equal_v<Unit1, Unit2>>>
    constexpr auto max(quantity<Rep1, Unit1> const& _first, quantity<Rep2, Unit2> const& _second) {
        using common_quantity = std::common_type_t<quantity<Rep1, Unit1>, quantity<Rep2, Unit2>>;

        using std::max;
        return make_quantity<Unit1>(max(common_quantity{_first}.count(), common_quantity{_second}.count()));
    }
}    // namespace randomcat::units