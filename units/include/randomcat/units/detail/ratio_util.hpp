#pragma once

#include <ratio>
#include <type_traits>

#include "randomcat/units/detail/static_math.hpp"

namespace randomcat::units::detail {
    template<typename T>
    struct is_ratio : std::false_type {};

    template<std::intmax_t Num, std::intmax_t Den>
    struct is_ratio<std::ratio<Num, Den>> : std::true_type {};

    template<typename T>
    inline auto constexpr is_ratio_v = is_ratio<T>::value;

    template<typename Ratio, int Pow, typename Enable = void>
    struct ratio_pow {
        using type = std::ratio_multiply<Ratio, typename ratio_pow<Ratio, Pow - 1>::type>;
    };

    template<typename Ratio>
    struct ratio_pow<Ratio, 0> {
        using type = std::ratio<1, 1>;
    };

    template<typename Ratio, int Pow>
    struct ratio_pow<Ratio, Pow, std::enable_if_t<Pow < 0>> {
        using type = std::ratio_divide<std::ratio<1, 1>, typename ratio_pow<Ratio, -Pow>::type>;
    };

    template<typename Ratio, int Pow>
    using ratio_pow_t = typename ratio_pow<Ratio, Pow>::type;

    template<typename R1, typename R2>
    struct ratio_gcd {
        static_assert(is_ratio_v<R1>, "Argument must be a std::ratio");
        static_assert(is_ratio_v<R2>, "Argument must be a std::ratio");

        using type = std::ratio<static_gcd_v<R1::num, R2::num>, (R1::den / static_gcd_v<R1::den, R2::den>) * R2::den>;
    };

    template<typename R1, typename R2>
    using ratio_gcd_t = typename ratio_gcd<R1, R2>::type;
}