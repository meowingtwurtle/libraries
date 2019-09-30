#pragma once

#include <type_traits>

#include "randomcat/units/detail/quantity_fwd.hpp"

namespace randomcat::units::detail {
    template<typename T>
    struct is_quantity : std::false_type {};

    template<typename... Ts>
    struct is_quantity<quantity<Ts...>> : std::true_type {};

    template<typename T>
    inline auto constexpr is_quantity_v = is_quantity<T>::value;

    template<typename T>
    struct quantity_info {
        static_assert(invalid<T>, "Argument must be a specialization of quantity");
    };

    template<typename Rep, typename Unit>
    struct quantity_info<quantity<Rep, Unit>> {
        using rep = Rep;
        using unit = Unit;
    };

    template<typename T>
    using quantity_rep_t = typename quantity_info<T>::rep;

    template<typename T>
    using quantity_unit_t = typename quantity_info<T>::unit;
}    // namespace randomcat::units::detail