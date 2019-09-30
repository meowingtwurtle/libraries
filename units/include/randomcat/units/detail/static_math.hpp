#pragma once

namespace randomcat::units::detail {
    template<typename T>
    struct integral_sum {
        template<T First, T Second>
        struct value_for {
            static auto constexpr value = First + Second;
        };
    };

    template<std::intmax_t X, typename = void>
    struct static_abs {
        static constexpr auto value = X;
    };

    template<std::intmax_t X>
        struct static_abs < X,
        std::enable_if_t<X<0>> {
        static constexpr auto value = -X;

        static_assert(value > 0, "Unable to negate value");
    };

    template<std::intmax_t X>
    inline auto constexpr static_abs_v = static_abs<X>::value;

    template<std::intmax_t P, std::intmax_t Q>
    struct static_gcd : static_gcd<Q, P % Q> {};

    template<std::intmax_t P>
    struct static_gcd<P, 0> {
        static auto constexpr value = static_abs_v<P>;
    };

    template<std::intmax_t Q>
    struct static_gcd<0, Q> {
        static auto constexpr value = static_abs_v<Q>;
    };

    template<std::intmax_t P, std::intmax_t Q>
    inline auto constexpr static_gcd_v = static_gcd<P, Q>::value;
}    // namespace randomcat::units::detail