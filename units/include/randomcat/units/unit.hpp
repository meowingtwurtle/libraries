#pragma once

#include <ratio>

#include "randomcat/units/detail/ratio_util.hpp"
#include "randomcat/units/detail/util.hpp"

namespace randomcat::units {
    // Users are not permitted to inspect the template arguments to unit
    template<typename TagCountMap, typename Scale = std::ratio<1, 1>>
    struct unit {
        static_assert(detail::is_ratio_v<Scale>, "Scale must be an instantiation of std::ratio");
    };
}