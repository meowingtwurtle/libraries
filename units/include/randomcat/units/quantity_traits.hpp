#pragma once

#include "randomcat/units/detail/quantity_util.hpp"

namespace randomcat::units {
    template<typename T>
    inline auto constexpr is_quantity_v = detail::is_quantity_v<T>;
}