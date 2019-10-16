#pragma once

#include "randomcat/units/detail/quantity_def.hpp"

namespace randomcat::units {
    template<typename Rep1, typename Unit1, typename Rep2, typename Unit2, typename = std::enable_if_t<unit_tags_are_equal_v<Unit1, Unit2>>>
    constexpr auto operator==(quantity<Rep1, Unit1> const& _first, quantity<Rep2, Unit2> const& _second) {
        using common = std::common_type_t<quantity<Rep1, Unit1>, quantity<Rep2, Unit2>>;

        return common{_first}.count() == common{_second}.count();
    }

    template<typename Rep1, typename Unit1, typename Rep2, typename Unit2, typename = std::enable_if_t<unit_tags_are_equal_v<Unit1, Unit2>>>
    constexpr auto operator!=(quantity<Rep1, Unit1> const& _first, quantity<Rep2, Unit2> const& _second) {
        using common = std::common_type_t<quantity<Rep1, Unit1>, quantity<Rep2, Unit2>>;

        return common{_first}.count() != common{_second}.count();
    }

    template<typename Rep1, typename Unit1, typename Rep2, typename Unit2, typename = std::enable_if_t<unit_tags_are_equal_v<Unit1, Unit2>>>
    constexpr auto operator<(quantity<Rep1, Unit1> const& _first, quantity<Rep2, Unit2> const& _second) {
        using common = std::common_type_t<quantity<Rep1, Unit1>, quantity<Rep2, Unit2>>;

        return common{_first}.count() < common{_second}.count();
    }

    template<typename Rep1, typename Unit1, typename Rep2, typename Unit2, typename = std::enable_if_t<unit_tags_are_equal_v<Unit1, Unit2>>>
    constexpr auto operator<=(quantity<Rep1, Unit1> const& _first, quantity<Rep2, Unit2> const& _second) {
        using common = std::common_type_t<quantity<Rep1, Unit1>, quantity<Rep2, Unit2>>;

        return common{_first}.count() <= common{_second}.count();
    }

    template<typename Rep1, typename Unit1, typename Rep2, typename Unit2, typename = std::enable_if_t<unit_tags_are_equal_v<Unit1, Unit2>>>
    constexpr auto operator>(quantity<Rep1, Unit1> const& _first, quantity<Rep2, Unit2> const& _second) {
        using common = std::common_type_t<quantity<Rep1, Unit1>, quantity<Rep2, Unit2>>;

        return common{_first}.count() > common{_second}.count();
    }

    template<typename Rep1, typename Unit1, typename Rep2, typename Unit2, typename = std::enable_if_t<unit_tags_are_equal_v<Unit1, Unit2>>>
    constexpr auto operator>=(quantity<Rep1, Unit1> const& _first, quantity<Rep2, Unit2> const& _second) {
        using common = std::common_type_t<quantity<Rep1, Unit1>, quantity<Rep2, Unit2>>;

        return common{_first}.count() >= common{_second}.count();
    }

    template<typename Rep1, typename Unit1, typename Rep2, typename Unit2, typename = std::enable_if_t<unit_tags_are_equal_v<Unit1, Unit2>>, typename common = std::common_type_t<quantity<Rep1, Unit1>, quantity<Rep2, Unit2>>>
    constexpr common min(quantity<Rep1, Unit1> const& _first, quantity<Rep2, Unit2> const& _second) {
        using std::min;
        return common(min(common{_first}.count(), common{_second}.count()));
    }

    template<typename Rep1, typename Unit1, typename Rep2, typename Unit2, typename = std::enable_if_t<unit_tags_are_equal_v<Unit1, Unit2>>, typename common_quantity = std::common_type_t<quantity<Rep1, Unit1>, quantity<Rep2, Unit2>>>
    constexpr common max(quantity<Rep1, Unit1> const& _first, quantity<Rep2, Unit2> const& _second) {
        using std::max;
        return common(max(common{_first}.count(), common{_second}.count()));
    }
}    // namespace randomcat::units