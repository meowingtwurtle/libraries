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

    template<typename Rep1, typename Unit1, typename Rep2, typename Unit2, typename = std::enable_if_t<unit_tags_are_equal_v<Unit1, Unit2>>>
    constexpr auto min(quantity<Rep1, Unit1> const& _first, quantity<Rep2, Unit2> const& _second) {
        using common_quantity = std::common_type_t<quantity<Rep1, Unit1>, quantity<Rep2, Unit2>>;

        using std::min;
        return units::make_quantity<Unit1>(min(common_quantity{_first}.count(), common_quantity{_second}.count()));
    }

    template<typename Rep1, typename Unit1, typename Rep2, typename Unit2, typename = std::enable_if_t<unit_tags_are_equal_v<Unit1, Unit2>>>
    constexpr auto max(quantity<Rep1, Unit1> const& _first, quantity<Rep2, Unit2> const& _second) {
        using common_quantity = std::common_type_t<quantity<Rep1, Unit1>, quantity<Rep2, Unit2>>;

        using std::max;
        return units::make_quantity<Unit1>(max(common_quantity{_first}.count(), common_quantity{_second}.count()));
    }
}    // namespace randomcat::units