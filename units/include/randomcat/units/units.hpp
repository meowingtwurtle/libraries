#pragma once

#include <ratio>
#include <chrono>
#include <cmath>

#include <randomcat/type_container/type_list.hpp>
#include <randomcat/type_container/type_map.hpp>
#include <randomcat/type_container/type_value_map.hpp>

namespace randomcat::units {
    namespace detail {
        template<typename... Ts>
        inline constexpr bool invalid = false;

        using tag_count = int16_t;

        using empty_tag_count_map = type_container::empty_type_value_map<tag_count>;

        template<typename T>
        struct is_ratio : std::false_type {};

        template<std::intmax_t Num, std::intmax_t Den>
        struct is_ratio<std::ratio<Num, Den>> : std::true_type {};

        template<typename T>
        inline auto constexpr is_ratio_v = is_ratio<T>::value;
    }

    // Users are not permitted to inspect the template arguments to unit
    template<typename TagCountMap, typename Scale = std::ratio<1, 1>>
    struct unit {
        static_assert(detail::is_ratio_v<Scale>, "Scale must be an instantiation of std::ratio");

    };

    template<typename Rep, typename Unit>
    class quantity;

    namespace detail {
        template<typename T>
        struct is_quantity : std::false_type {};

        template<typename... Ts>
        struct is_quantity<quantity<Ts...>> : std::true_type {};

        template<typename T>
        inline auto constexpr is_quantity_v = is_quantity<T>::value;

        template<typename T>
        struct is_unit : std::false_type {};

        template<typename... Ts>
        struct is_unit<unit<Ts...>> : std::true_type {};

        template<typename T>
        inline auto constexpr is_unit_v = is_unit<T>::value;

        template<typename T>
        struct unit_info { static_assert(invalid<T>, "Argument must be a unit"); };

        template<typename TagCountMap, typename Scale>
        struct unit_info<unit<TagCountMap, Scale>> {
            using tag_counts = TagCountMap;
            using scale = Scale;
        };

        template<typename T>
        using unit_tag_counts_t = typename unit_info<T>::tag_counts;

        template<typename T>
        using unit_scale_t = typename unit_info<T>::scale;

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

        template<typename T>
        struct integral_sum {
            template<T First, T Second>
            struct value_for {
                static auto constexpr value = First + Second;
            };
        };

        template<typename M1, typename M2>
        using tag_count_map_sum_t = type_container::type_value_map_merge_t<M1, M2, integral_sum<tag_count>::value_for, 0>;

        template<typename U1, typename U2>
        using product_unit_t = unit<tag_count_map_sum_t<unit_tag_counts_t<U1>, unit_tag_counts_t<U2>>, std::ratio_multiply<unit_scale_t<U1>, unit_scale_t<U2>>>;

        template<std::intmax_t X, typename = void>
        struct static_abs {
            static constexpr auto value = X;
        };

        template<std::intmax_t X>
        struct static_abs<X, std::enable_if_t<X < 0>> {
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

        template<typename R1, typename R2>
        struct ratio_gcd {
            static_assert(is_ratio_v<R1>, "Argument must be a std::ratio");
            static_assert(is_ratio_v<R2>, "Argument must be a std::ratio");

            using type = std::ratio<static_gcd_v<R1::num, R2::num>, (R1::den / static_gcd_v<R1::den, R2::den>) * R2::den>;
        };

        template<typename R1, typename R2>
        using ratio_gcd_t = typename ratio_gcd<R1, R2>::type;

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

        template<typename Key, auto Value>
        struct node_has_zero_value : std::bool_constant<Value == 0> {};

        template<typename U>
        inline auto constexpr unit_is_unitless_v = type_container::type_value_map_is_empty_v<unit_tag_counts_t<U>> || type_container::type_value_map_all_match_v<unit_tag_counts_t<U>, node_has_zero_value>;

        template<typename M>
        using tag_map_remove_zero_tags_t = type_container::type_value_map_remove_if_t<M, node_has_zero_value>;

        template<typename Unit, typename Scale>
        using unit_with_scale_t = unit<unit_tag_counts_t<Unit>, Scale>;

        template<typename Unit>
        using unit_without_scale_t = unit_with_scale_t<Unit, std::ratio<1, 1>>;

        template<typename... Tags>
        using unit_from_tags_t = unit<type_container::type_list<Tags...>, std::ratio<1, 1>>;

        template<typename Map0, typename Map1>
        inline auto constexpr tag_maps_are_equal_v =  type_container::type_value_maps_are_equal_v<detail::tag_map_remove_zero_tags_t<Map0>, detail::tag_map_remove_zero_tags_t<Map1>>;
    }

    template<typename Tag>
    using simple_unit = unit<detail::empty_tag_count_map::set<Tag, 1>>;

    template<typename Unit, detail::tag_count Pow>
    using power_unit = unit<detail::tag_count_map_multiply_t<detail::unit_tag_counts_t<Unit>, Pow>, detail::ratio_pow_t<detail::unit_scale_t<Unit>, Pow>>;

    template<typename Unit>
    using inverse_unit = power_unit<Unit, -1>;

    template<typename Unit, typename Scale>
    using scale_unit = unit<detail::unit_tag_counts_t<Unit>, std::ratio_multiply<detail::unit_scale_t<Unit>, Scale>>;

    template<typename Unit1, typename Unit2>
    using product_unit = detail::product_unit_t<Unit1, Unit2>;

    template<typename Unit1, typename Unit2>
    using quotient_unit = product_unit<Unit1, inverse_unit<Unit2>>;

    using no_unit = unit<detail::empty_tag_count_map, std::ratio<1, 1>>;

    template<typename Unit1, typename Unit2>
    inline auto constexpr unit_tags_are_equal_v = detail::tag_maps_are_equal_v<detail::unit_tag_counts_t<Unit1>, detail::unit_tag_counts_t<Unit2>>;

    template<typename Unit1, typename Unit2>
    inline auto constexpr units_are_equal_v = unit_tags_are_equal_v<Unit1, Unit2> && std::ratio_equal_v<detail::unit_scale_t<Unit2>, detail::unit_scale_t<Unit2>>;

    template<typename Unit>
    inline auto constexpr unit_is_unitless_v = detail::unit_is_unitless_v<Unit>;

    template<typename T>
    inline auto constexpr is_quantity_v = detail::is_quantity_v<T>;

    template<typename ToQuantity, typename FromRep, typename FromUnit, typename = std::enable_if_t<unit_tags_are_equal_v<FromUnit, detail::quantity_unit_t<ToQuantity>>>>
    inline constexpr ToQuantity quantity_cast(quantity<FromRep, FromUnit> const& _value) {
        using ToUnit = detail::quantity_unit_t<ToQuantity>;
        using ToRep = detail::quantity_rep_t<ToQuantity>;

        using math_type = std::conditional_t<std::is_floating_point_v<ToRep> || std::is_floating_point_v<FromRep>, long double, std::intmax_t>;
        using scale_ratio = std::ratio_divide<detail::unit_scale_t<FromUnit>, detail::unit_scale_t<ToUnit>>;

        auto fromCount = static_cast<math_type>(_value.count());

        if constexpr (std::chrono::treat_as_floating_point_v<math_type>) {
            return ToQuantity{static_cast<ToRep>(fromCount / scale_ratio::den) * scale_ratio::num};
        } else {
            return ToQuantity{static_cast<ToRep>((fromCount * scale_ratio::num) / scale_ratio::den)};
        }
    }

    // Client code that directly uses these tags has undefined behavior
    namespace detail::default_tags {
        struct time_tag {};
        struct distance_tag {};
        struct mass_tag {};
        struct temperature_tag {};
        struct current_tag {};
        struct luminous_intensity_tag {};
        struct angle_tag {};
    }
    
    namespace detail {
        template<typename Derived, typename Enable = void>
        class chrono_quantity_base {
        public:
            inline static constexpr auto is_time = false;
            using chrono_type = void;
        };
        
        using time_tag_map = detail::empty_tag_count_map::set<detail::default_tags::time_tag, 1>;
        
        template<typename Rep, typename Unit>
        class chrono_quantity_base<quantity<Rep, Unit>, std::enable_if_t<detail::tag_maps_are_equal_v<detail::unit_tag_counts_t<Unit>, time_tag_map>>> {
        private:
            constexpr auto derived_this() const noexcept {
                return static_cast<quantity<Rep, Unit> const*>(this);
            }

        public:
            using chrono_type = std::chrono::duration<Rep, detail::unit_scale_t<Unit>>;
            inline static constexpr auto is_time = true;

            constexpr chrono_type as_chrono() const noexcept(noexcept(chrono_type(derived_this()->count()))) {
                return chrono_type(derived_this()->count());
            }

            constexpr operator chrono_type() const noexcept(noexcept(as_chrono())) {
                return as_chrono();
            }
        };

        template<typename Derived, typename Enable = void>
        class unitless_quantity_base {};
        
        template<typename Rep, typename Unit>
        class unitless_quantity_base<quantity<Rep, Unit>, std::enable_if_t<unit_is_unitless_v<Unit>>> {
        private:
            using derived = quantity<Rep, Unit>;
            using scale = unit_scale_t<Unit>;

            constexpr auto derived_this() const noexcept {
                return static_cast<derived const*>(this);
            }
        
        public:
            constexpr operator auto() const noexcept(noexcept(derived_this()->count())) {
                static_assert(std::chrono::treat_as_floating_point_v<Rep> || (scale::den == 1), "Possible loss of precision in conversion.");

                return derived_this()->count() * scale::num / scale::den;
            }
        };
    }

    template<typename Rep, typename Unit>
    class quantity : public detail::chrono_quantity_base<quantity<Rep, Unit>>, public detail::unitless_quantity_base<quantity<Rep, Unit>> {
        static_assert(detail::is_unit_v<Unit>, "Unit must be a unit");
        static_assert(std::is_same_v<Rep, std::remove_cv_t<std::remove_reference_t<Rep>>>, "Rep cannot be cv qualified or a reference");
        static_assert(!is_quantity_v<Rep>, "Rep cannot be a quantity");

    private:
        using chrono_base = detail::chrono_quantity_base<quantity<Rep, Unit>>;
        inline static constexpr auto is_time = chrono_base::is_time;
        using chrono_type = typename chrono_base::chrono_type;

    public:
        using rep = Rep;
        using unit = Unit;

        static_assert(std::is_nothrow_copy_constructible_v<Rep>);
        static_assert(std::is_nothrow_move_constructible_v<Rep>);
        static_assert(noexcept(std::declval<Rep>() + std::declval<Rep>()));
        static_assert(noexcept(std::declval<Rep>() - std::declval<Rep>()));
        static_assert(noexcept(std::declval<Rep>() * std::declval<Rep>()));
        static_assert(noexcept(std::declval<Rep>() / std::declval<Rep>()));
        static_assert(noexcept(std::declval<std::conditional_t<!std::is_floating_point_v<Rep>, Rep, int>>() % std::declval<int>()));
        static_assert(noexcept(-std::declval<Rep>()));
        
        constexpr explicit quantity(Rep _value) noexcept : m_value(std::move(_value)) {}
        
        template<typename Rep2, typename Unit2, typename = std::enable_if_t<unit_tags_are_equal_v<Unit, Unit2> && (std::chrono::treat_as_floating_point_v<Rep> || (std::ratio_divide<detail::unit_scale_t<Unit2>, detail::unit_scale_t<Unit>>::den == 1))>>
        constexpr quantity(quantity<Rep2, Unit2> const& _other) noexcept
        : m_value{quantity_cast<quantity>(_other).count()}
        {}
        
        template<bool Enable = is_time>
        /* implicit */ constexpr quantity(std::enable_if_t<Enable, chrono_type> const& _asChrono) noexcept
        : m_value{_asChrono.count()} {}

        constexpr Rep count() const noexcept {
            return m_value;
        }

        template<typename OtherUnit, typename = std::enable_if_t<units_are_equal_v<Unit, OtherUnit>>>
        constexpr quantity& operator+=(quantity<Rep, OtherUnit> const& _other) & noexcept {
            static_assert(noexcept(m_value += _other.count()));

            m_value += _other.count();
            return *this;
        }

        template<typename OtherRep, typename OtherUnit, typename = std::enable_if_t<units_are_equal_v<Unit, OtherUnit>>>
        constexpr quantity& operator-=(quantity<OtherRep, OtherUnit> const& _other) & noexcept {
            static_assert(noexcept(m_value -= _other.count()));

            m_value -= _other.count();
            return *this;
        }

        template<typename OtherRep, typename = std::enable_if_t<std::is_arithmetic_v<OtherRep>>>
        constexpr quantity& operator*=(OtherRep const& _other) & noexcept {
            static_assert(noexcept(m_value *= _other));
            
            m_value *= _other;
            return *this;
        }

        template<typename OtherRep, typename R = Rep, typename = std::enable_if_t<std::is_arithmetic_v<OtherRep> && std::is_floating_point_v<R>>>
        constexpr quantity& operator/=(OtherRep const& _other) & noexcept {
            static_assert(noexcept(m_value /= _other));

            m_value /= _other;
            return *this;
        }

        template<typename OtherRep, typename = std::enable_if_t<std::is_integral_v<OtherRep>>>
        constexpr quantity& operator%=(OtherRep const& _other) & noexcept {
            static_assert(noexcept(m_value %= _other));

            m_value %= _other;
            return *this;
        }

        constexpr quantity operator+() const noexcept {
            return *this;
        }

        constexpr quantity operator-() const noexcept {
            return quantity{-m_value};
        }

    private:
        Rep m_value;
    };
}

namespace std {
    template<typename Rep1, typename Unit1, typename Rep2, typename Unit2>
    struct common_type<randomcat::units::quantity<Rep1, Unit1>, randomcat::units::quantity<Rep2, Unit2>> {
    private:
        using scale1 = randomcat::units::detail::unit_scale_t<Unit1>;
        using scale2 = randomcat::units::detail::unit_scale_t<Unit2>;

        static_assert(randomcat::units::unit_tags_are_equal_v<Unit1, Unit2>, "Units must have the same tags");

    public:
        using type = randomcat::units::quantity<std::common_type_t<Rep1, Rep2>, randomcat::units::unit<randomcat::units::detail::unit_tag_counts_t<Unit1>, randomcat::units::detail::ratio_gcd_t<scale1, scale2>>>;
    };
}

namespace randomcat::units {
    namespace detail {
        template<
            typename Rep1,
            typename Unit1,
            typename Rep2,
            typename Unit2,
            typename = std::enable_if_t<unit_tags_are_equal_v<Unit1, Unit2>>,
            typename Compare
        >
        constexpr decltype(auto) with_common_counts(quantity<Rep1, Unit1> const& _first, quantity<Rep2, Unit2> const& _second, Compare _comp) noexcept {
            using common_quantity = std::common_type_t<quantity<Rep1, Unit1>, quantity<Rep2, Unit2>>;
            using common_rep = quantity_rep_t<common_quantity>;

            static_assert(std::is_convertible_v<std::invoke_result_t<Compare, common_rep, common_rep>, bool>);

            return _comp(common_quantity{_first}.count(), common_quantity{_second}.count());
        }
    }

#define RC_QUANTITY_COMPARE_OP(OP) \
    \
    template<typename Rep1, typename Unit1, typename Rep2, typename Unit2> \
    constexpr bool operator OP (quantity<Rep1, Unit1> const& _first, quantity<Rep2, Unit2> const& _second) noexcept { \
        return detail::with_common_counts(_first, _second, [](auto x, auto y) constexpr noexcept { return x OP y; }); \
    }

    RC_QUANTITY_COMPARE_OP(==);
    RC_QUANTITY_COMPARE_OP(!=);
    RC_QUANTITY_COMPARE_OP(< );
    RC_QUANTITY_COMPARE_OP(<=);
    RC_QUANTITY_COMPARE_OP(> );
    RC_QUANTITY_COMPARE_OP(>=);

#undef RC_QUANTITY_COMPARE_OP

    template<typename Unit, typename Rep>
    constexpr auto make_quantity(Rep _value) noexcept {
        return quantity<Rep, Unit>(_value);
    }

    template<typename Rep, typename Unit>
    constexpr quantity<Rep, Unit> abs(quantity<Rep, Unit> _q) {
        using std::abs;
        return quantity{abs(_q.count())};
    }
    
    template<
        typename Rep1,
        typename Unit1,
        typename Rep2,
        typename Unit2,
        typename = std::enable_if_t<unit_tags_are_equal_v<Unit1, Unit2>>
    >
    constexpr auto min(quantity<Rep1, Unit1> const& _first, quantity<Rep2, Unit2> const& _second) {
        using common_quantity = std::common_type_t<quantity<Rep1, Unit1>, quantity<Rep2, Unit2>>;
        using common_rep = detail::quantity_rep_t<common_quantity>;
        
        using std::min;
        return common_quantity{min(common_rep{common_quantity{_first}.count()}, common_rep{common_quantity{_second}.count()})};
    }

    template<
        typename Rep1,
        typename Unit1,
        typename Rep2,
        typename Unit2,
        typename = std::enable_if_t<unit_tags_are_equal_v<Unit1, Unit2>>
    >
    constexpr auto max(quantity<Rep1, Unit1> const& _first, quantity<Rep2, Unit2> const& _second) {
        using common_quantity = std::common_type_t<quantity<Rep1, Unit1>, quantity<Rep2, Unit2>>;
        using common_rep = detail::quantity_rep_t<common_quantity>;

        using std::max;
        return common_quantity{max(common_rep{common_quantity{_first}.count()}, common_rep{common_quantity{_second}.count()})};
    }

    template<std::intmax_t Power, typename Rep, typename Unit>
    constexpr auto pow(quantity<Rep, Unit> const& _quantity) {
        using std::pow;
        return units::make_quantity<power_unit<Unit, Power>>(pow(_quantity.count(), Power));
    }

    template<typename Rep1, typename Unit1, typename Rep2, typename Unit2, typename = std::enable_if_t<unit_tags_are_equal_v<Unit1, Unit2>>>
    constexpr std::common_type_t<quantity<Rep1, Unit1>, quantity<Rep2, Unit2>> operator+(quantity<Rep1, Unit1> const& _first, quantity<Rep2, Unit2> const& _second) noexcept {
        using common = std::common_type_t<quantity<Rep1, Unit1>, quantity<Rep2, Unit2>>;
        return common(common(_first).count() + common(_second).count());
    }

    template<typename Rep1, typename Unit1, typename Rep2, typename Unit2>
    constexpr quantity<std::common_type_t<Rep1, Rep2>, product_unit<Unit1, Unit2>> operator*(quantity<Rep1, Unit1> const& _first, quantity<Rep2, Unit2> const& _second) noexcept {
        using common_rep = std::common_type_t<Rep1, Rep2>;
        using result = quantity<common_rep, product_unit<Unit1, Unit2>>;
        return result(static_cast<common_rep>(_first.count()) * static_cast<common_rep>(_second.count()));
    }

    template<typename ValueRep, typename QuantityRep, typename Unit, typename = std::enable_if_t<!is_quantity_v<ValueRep>>>
    constexpr decltype(auto) operator*(ValueRep const& _val, quantity<QuantityRep, Unit> const& _quantity) noexcept {
        return _quantity * quantity<ValueRep, no_unit>{_val};
    }

    template<typename ValueRep, typename QuantityRep, typename Unit, typename = std::enable_if_t<!is_quantity_v<ValueRep>>>
    constexpr decltype(auto) operator*(quantity<QuantityRep, Unit> const& _quantity, ValueRep const& _val) noexcept {
        return _val * _quantity;
    }

    template<typename Rep1, typename Unit1, typename Rep2, typename Unit2>
    constexpr decltype(auto) operator-(quantity<Rep1, Unit1> const& _first, quantity<Rep2, Unit2> const& _second) noexcept {
        using common = std::common_type_t<quantity<Rep1, Unit1>, quantity<Rep2, Unit2>>;
        return common(common(_first).count() - common(_second).count());
    }

    template<typename Rep1, typename Unit1, typename Rep2, typename Unit2>
    constexpr quantity<std::common_type_t<Rep1, Rep2>, quotient_unit<detail::unit_without_scale_t<Unit1>, detail::unit_without_scale_t<Unit2>>> operator/(quantity<Rep1, Unit1> const& _first, quantity<Rep2, Unit2> const& _second) noexcept {
        using common_rep = std::common_type_t<Rep1, Rep2>;
        using result = quantity<common_rep, quotient_unit<detail::unit_without_scale_t<Unit1>, detail::unit_without_scale_t<Unit2>>>;

        using scale_gcd = detail::ratio_gcd_t<detail::unit_scale_t<Unit1>, detail::unit_scale_t<Unit2>>;
        using first_scaled = quantity<common_rep, unit<detail::unit_tag_counts_t<Unit1>, scale_gcd>>;
        using second_scaled = quantity<common_rep, unit<detail::unit_tag_counts_t<Unit2>, scale_gcd>>;

        return result{static_cast<common_rep>(first_scaled{_first}.count()) / static_cast<common_rep>(second_scaled{_second}.count())};
    }

    template<typename ValueRep, typename QuantityRep, typename QuantityUnit, typename = std::enable_if_t<!is_quantity_v<ValueRep>>>
    constexpr quantity<std::common_type_t<ValueRep, QuantityRep>, QuantityUnit> operator/(quantity<QuantityRep, QuantityUnit> const& _q, ValueRep const& _v) {
        using common_rep = std::common_type_t<ValueRep, QuantityRep>;
        using result = quantity<common_rep, QuantityUnit>;

        return result{static_cast<common_rep>(_q.count()) / static_cast<common_rep>(_v)};
    }
}
