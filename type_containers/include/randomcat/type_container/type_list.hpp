#pragma once

#include <type_traits>
#include <memory>
#include <tuple>

namespace randomcat::type_container {
    // The template arguments are guaranteed to be the nested types, in order.
    template<typename... Ts> struct type_list;
    using empty_type_list = type_list<>;

    namespace type_list_detail {
        template<typename First>
        struct is_same_as {
            template<typename Second>
            struct value_for {
                static constexpr auto value = std::is_same_v<First, Second>;
            };
        };

        template<template<typename> typename Condition>
        struct invert {
            template<typename Arg>
            struct value_for {
                static constexpr bool value = !bool(Condition<Arg>::value);
            };
        };
    }
    
    namespace type_list_detail {
        using type_list_index_t = signed long;
        using type_list_size_t = signed long;

        template<typename T>
        struct is_type_list : std::false_type {};

        template<typename... Ts>
        struct is_type_list<type_list<Ts...>> : std::true_type {};

        template<typename T>
        inline constexpr bool is_type_list_v = is_type_list<T>::value;

        template<typename... Ts>
        inline constexpr auto invalid_v = false;

        template<template<typename...> typename Condition, typename... Args>
        inline constexpr auto eval_v = Condition<Args...>::value;

        template<typename L>
        struct type_list_size {
            static_assert(invalid_v<L>, "Argument must be a type_list");
        };

        template<typename... Ts>
        struct type_list_size<type_list<Ts...>> {
            static constexpr type_list_size_t value = sizeof...(Ts);
        };

        template<typename L>
        inline constexpr auto type_list_size_v = type_list_size<L>::value;

        template<typename L, type_list_index_t I>
        inline constexpr bool type_list_index_is_valid_v = (I >= 0) && (I < type_list_size_v<L>);

        template<typename L, type_list_index_t I>
        inline constexpr bool type_list_index_is_valid_or_end_v =
            type_list_index_is_valid_v<L, I> || I == type_list_size_v<L>;

        template<typename L1, typename L2>
        struct type_list_merge {
            static_assert(invalid_v<L1, L2>, "Arguments must be type_lists");
        };

        template<typename... L1_Ts, typename... L2_Ts>
        struct type_list_merge<type_list<L1_Ts...>, type_list<L2_Ts...>> {
            using type = type_list<L1_Ts..., L2_Ts...>;
        };

        template<typename L1, typename L2>
        using type_list_merge_t = typename type_list_merge<L1, L2>::type;

        template<typename L, typename T>
        using type_list_push_back_t = type_list_merge_t<L, type_list<T>>;

        template<typename L, typename T>
        using type_list_push_front_t = type_list_merge_t<type_list<T>, L>;

        template<typename L, type_list_index_t I>
        struct type_list_at {
            static_assert(invalid_v<L>, "Argument must be a type_list");
        };

        template<typename T0, typename... Ts, type_list_index_t I>
        struct type_list_at<type_list<T0, Ts...>, I> {
            static_assert(0 <= I && I < 1 + sizeof...(Ts), "Invalid type_list index");

            using type = typename type_list_at<type_list<Ts...>, I - 1>::type;
        };

        template<typename T0, typename... Ts>
        struct type_list_at<type_list<T0, Ts...>, 0> {
            using type = T0;
        };

        template<typename L, type_list_index_t I>
        using type_list_at_t = typename type_list_at<L, I>::type;

        template<typename L, type_list_index_t Begin, type_list_index_t End>
        struct type_list_sub_list {
            static_assert(is_type_list_v<L>, "Argument must be a type_list");
            static_assert(type_list_index_is_valid_v<L, Begin>, "Invalid value for begin index");
            static_assert(type_list_index_is_valid_or_end_v<L, End>, "Invalid value for end index");

            using type = type_list_push_front_t<
                typename type_list_sub_list<L, Begin + 1, End>::type,
                type_list_at_t<L, Begin>>;
        };

        template<typename L, type_list_index_t I>
        struct type_list_sub_list<L, I, I> {
            static_assert(is_type_list_v<L>, "Argument must be a type_list");
            static_assert(type_list_index_is_valid_or_end_v<L, I>, "Invalid value for index");

            using type = empty_type_list;
        };

        template<typename L, type_list_index_t B, type_list_index_t E>
        using type_list_sub_list_t = typename type_list_sub_list<L, B, E>::type;

        template<typename L, typename T, type_list_index_t I>
        struct type_list_insert_at {
            static_assert(is_type_list_v<L>, "Argument must be a type_list");
            static_assert(type_list_index_is_valid_or_end_v<L, I>, "Invalid value for index");

        private:
            using front = type_list_sub_list_t<L, 0, I>;
            using front_with_elem = type_list_push_back_t<front, T>;
            using back = type_list_sub_list_t<L, I, type_list_size_v<L>>;
            using whole = type_list_merge_t<front_with_elem, back>;
        public:
            using type = whole;
        };

        template<typename L, typename T, type_list_index_t I>
        using type_list_insert_at_t = typename type_list_insert_at<L, T, I>::type;

        template<typename L>
        using type_list_tail_t = type_list_sub_list_t<L, 1, type_list_size_v<L>>;

        template<typename L, template<typename> typename Condition>
        struct type_list_remove_if {
            static_assert(is_type_list_v<L>, "Argument must be a type_list");

        private:
            using tail = type_list_tail_t<L>;
            using recurse_tail = typename type_list_remove_if<tail, Condition>::type;
            using front = type_list_at_t<L, 0>;

        public:
            using type = std::conditional_t<
                eval_v<Condition, front>,
                recurse_tail,
                type_list_push_front_t<recurse_tail, front>
            >;
        };

        template<template<typename> typename Condition>
        struct type_list_remove_if<empty_type_list, Condition> {
            using type = empty_type_list;
        };

        template<typename L, template<typename> typename C>
        using type_list_remove_if_t = typename type_list_remove_if<L, C>::type;
        
        template<typename L, typename T>
        using type_list_remove_t = type_list_remove_if_t<L, is_same_as<T>::template value_for>;

        template<typename L, template<typename> typename Condition>
        struct type_list_count_if {
            static_assert(is_type_list_v<L>, "Argument must be a type_list");

        private:
            using tail = type_list_tail_t<L>;
            using front = type_list_at_t<L, 0>;

            static constexpr long tail_recurse = type_list_count_if<tail, Condition>::value;

        public:
            static constexpr long value = (eval_v<Condition, front> ? 1 : 0) + tail_recurse;
        };

        template<template<typename> typename Condition>
        struct type_list_count_if<empty_type_list, Condition> {
            static constexpr long value = 0;
        };

        template<typename L, template<typename> typename Condition>
        inline constexpr auto type_list_count_if_v = type_list_count_if<L, Condition>::value;
        
        template<typename L, typename T>
        inline auto constexpr type_list_count_v = type_list_count_if_v<L, is_same_as<T>::template value_for>;
        
        template<typename L, type_list_index_t I, typename Default, typename Enable = void>
        struct type_list_at_or {
            static_assert(is_type_list_v<L>, "Argument must be a type_list");

            using type = Default;
        };

        template<typename L, type_list_index_t I, typename Default>
        struct type_list_at_or<L, I, Default, std::enable_if_t<type_list_index_is_valid_v<L, I>>> {
            using type = type_list_at_t<L, I>;
        };

        template<typename L, type_list_index_t I, typename Default>
        using type_list_at_or_t = typename type_list_at_or<L, I, Default>::type;

        template<typename L, template<typename> typename Transform>
        struct type_list_transform {
            static_assert(invalid_v<L>, "Argument must be a type_list");
        };

        template<typename... LArgs, template<typename> typename Transform>
        struct type_list_transform<type_list<LArgs...>, Transform> {
            using type = type_list<typename Transform<LArgs>::type...>;
        };

        template<typename L, template<typename> typename Transform>
        using type_list_transform_t = typename type_list_transform<L, Transform>::type;
        
        template<typename L>
        struct type_list_remove_duplicates {
            static_assert(is_type_list_v<L>, "Argument must be a type_list");
            
            using first = type_list_at_t<L, 0>;
            using rest = type_list_sub_list_t<L, 1, type_list_size_v<L>>;
            using rest_without_first = type_list_remove_t<rest, first>;
            using type = type_list_push_front_t<typename type_list_remove_duplicates<rest_without_first>::type, first>;
        };
        
        template<>
        struct type_list_remove_duplicates<empty_type_list> {
            using type = empty_type_list;
        };
        
        template<typename L>
        using type_list_remove_duplicates_t = typename type_list_remove_duplicates<L>::type;
        
        template<typename T>
        struct require_type_list {
            static_assert(is_type_list_v<T>, "Argument must be a type_list");
            using type = T;
        };
        
        template<typename T>
        using require_type_list_t = typename require_type_list<T>::type;
        
        template<typename L1, typename L2, typename Enable = void>
        struct type_list_equal_unordered {
            static_assert(is_type_list_v<L1>, "Argument must be a type_list");
            static_assert(is_type_list_v<L2>, "Argument must be a type_list");
            
            using test_type = type_list_at_t<L1, 0>;
            static auto constexpr L1_test_count = type_list_count_v<L1, test_type>;
            static auto constexpr L2_test_count = type_list_count_v<L2, test_type>;
            using L1_no_test = type_list_remove_t<L1, test_type>;
            using L2_no_test = type_list_remove_t<L2, test_type>;
            static auto constexpr value = L1_test_count == L2_test_count ? type_list_equal_unordered<L1_no_test, L2_no_test>::value : false;
        };
        
        template<>
        struct type_list_equal_unordered<empty_type_list, empty_type_list> : std::true_type {};
        
        template<typename L1, typename L2>
        struct type_list_equal_unordered<L1, L2, std::enable_if_t<type_list_size_v<L1> != type_list_size_v<L2>>> : std::false_type {};
        
        template<typename L1, typename L2>
        inline auto constexpr type_list_equal_unordered_v = type_list_equal_unordered<L1, L2>::value;

        template<typename L>
        struct is_element_of {
            template<typename Arg> 
            struct value_for {
                static constexpr bool value = type_list_count_v<L, Arg> > 0;
            };
        };
        
        template<typename L, template<typename...> typename TT>
        struct type_list_substitute_args_into_template;

        template<typename... LElems, template<typename...> typename TT>
        struct type_list_substitute_args_into_template<type_list<LElems...>, TT> {
            using type = TT<LElems...>;
        };
        
        template<typename L, template<typename...> typename TT>
        using type_list_substitute_args_into_template_t = type_list_substitute_args_into_template<L, TT>;
        
        template<typename L, typename T>
        struct type_list_first_index_of;
        
        template<typename... LRest, typename T>
        struct type_list_first_index_of<type_list<T, LRest...>, T> {
            static constexpr auto value = 0;
        };
        
        template<typename LFirst, typename... LRest, typename T>
        struct type_list_first_index_of<type_list<LFirst, LRest...>, T> {
            static constexpr auto value = 1 + type_list_first_index_of<type_list<LRest...>, T>::value;
        };
        
        template<typename L, typename T>
        inline constexpr auto type_list_first_index_of_v = type_list_first_index_of<L, T>::value;
    }
    
    template<typename L, type_list_detail::type_list_index_t I>
    using type_list_at_t = type_list_detail::type_list_at_t<type_list_detail::require_type_list_t<L>, I>;
    
    template<typename L, typename T>
    using type_list_push_back_t = type_list_detail::type_list_push_back_t<type_list_detail::require_type_list_t<L>, T>;

    template<typename L, typename T>
    using type_list_push_front_t = type_list_detail::type_list_push_front_t<type_list_detail::require_type_list_t<L>, T>;

    template<typename L1, typename L2>
    using type_list_concat_t = type_list_detail::type_list_merge_t<type_list_detail::require_type_list_t<L1>, type_list_detail::require_type_list_t<L2>>;
    
    template<typename L, type_list_detail::type_list_index_t B, type_list_detail::type_list_index_t E>
    using type_list_sub_list_t = type_list_detail::type_list_sub_list_t<type_list_detail::require_type_list_t<L>, B, E>;
    
    template<typename L, typename T, type_list_detail::type_list_index_t I>
    using type_list_insert_at_t = type_list_detail::type_list_insert_at_t<type_list_detail::require_type_list_t<L>, T, I>;
    
    template<typename L, template<typename> typename Condition>
    using type_list_remove_if_t = type_list_detail::type_list_remove_if_t<type_list_detail::require_type_list_t<L>, Condition>;

    template<typename L, typename T>
    using type_list_remove_t = type_list_detail::type_list_remove_t<type_list_detail::require_type_list_t<L>, T>;

    template<typename L, template<typename> typename Condition>
    using type_list_retain_if_t = type_list_remove_if_t<L, type_list_detail::invert<Condition>::template value_for>;

    template<typename L>
    inline auto constexpr type_list_size_v = type_list_detail::type_list_size_v<type_list_detail::require_type_list_t<L>>;

    template<typename L, template<typename> typename Condition>
    inline auto constexpr type_list_count_if_v = type_list_detail::type_list_count_if_v<type_list_detail::require_type_list_t<L>, Condition>;
    
    template<typename L, typename T>
    inline auto constexpr type_list_count_v = type_list_detail::type_list_count_v<L, T>;

    template<typename L, template<typename> typename Condition>
    inline auto constexpr type_list_all_match_v = type_list_size_v<L> == type_list_count_if_v<L, Condition>;

    template<typename L, template<typename> typename Condition>
    inline auto constexpr type_list_none_match_v = type_list_count_if_v<L, Condition> == 0;

    template<typename L, template<typename> typename Condition>
    inline auto constexpr type_list_any_match_v = type_list_count_if_v<L, Condition> > 0;
    
    template<typename L, type_list_detail::type_list_index_t I, typename Default>
    using type_list_at_or_t = type_list_detail::type_list_at_or_t<type_list_detail::require_type_list_t<L>, I, Default>;
    
    template<typename T>
    inline auto constexpr is_type_list_v = type_list_detail::is_type_list_v<T>;
    
    template<typename L, template<typename> typename Transform>
    using type_list_transform_t = type_list_detail::type_list_transform_t<type_list_detail::require_type_list_t<L>, Transform>;
    
    template<typename L>
    using type_list_without_duplicates_t = type_list_detail::type_list_remove_duplicates_t<type_list_detail::require_type_list_t<L>>;
    
    template<typename L>
    inline auto constexpr type_list_has_duplicates_v = !std::is_same_v<type_list_detail::require_type_list_t<L>, type_list_without_duplicates_t<L>>;
    
    template<typename L1, typename L2>
    inline auto constexpr type_lists_have_matches_v = type_list_has_duplicates_v<type_list_concat_t<type_list_without_duplicates_t<L1>, type_list_without_duplicates_t<L2>>>;
    
    template<typename L1, typename L2>
    inline auto constexpr type_lists_are_equal_unordered_v = type_list_detail::type_list_equal_unordered_v<type_list_detail::require_type_list_t<L1>, type_list_detail::require_type_list_t<L2>>;
    
    template<typename L, typename T>
    inline auto constexpr type_list_contains_v = type_list_count_v<L, T> > 0;
    
    template<typename L>
    inline auto constexpr type_list_is_empty_v = type_list_size_v<L> == 0;
    
    template<typename Lsub, typename Llarge>
    inline auto constexpr type_list_is_sub_list_of_v = type_list_is_empty_v<type_list_remove_if_t<Lsub, type_list_detail::is_element_of<type_list_detail::require_type_list_t<Llarge>>::template value_for>>;
    
    template<typename L, typename T>
    inline auto constexpr type_list_first_index_of_v = type_list_detail::type_list_first_index_of_v<type_list_detail::require_type_list_t<L>, T>;
    
    template<typename L, template<typename...> typename TT>
    using type_list_substitute_args_into_template_t = type_list_detail::type_list_substitute_args_into_template_t<type_list_detail::require_type_list_t<L>, TT>;
    
    namespace type_list_detail {
        template<typename ThisL>
        struct type_list_base {
        public:
            static constexpr long size = type_list_size_v<ThisL>;
            static constexpr bool is_empty = (size == 0);
            static constexpr bool is_not_empty = !is_empty;

            template<typename T>
            using push_back = type_list_push_back_t<ThisL, T>;

            template<typename T>
            using push_front = type_list_push_front_t<ThisL, T>;

            template<typename NewBackL>
            using append_list = type_list_concat_t<ThisL, NewBackL>;

            template<typename NewFrontL>
            using prepend_list = type_list_concat_t<NewFrontL, ThisL>;

            template<type_list_detail::type_list_index_t B, type_list_detail::type_list_index_t E>
            using sub_list = type_list_sub_list_t<ThisL, B, E>;

            template<typename T, type_list_detail::type_list_index_t I>
            using insert_at = type_list_insert_at_t<ThisL, T, I>;

            template<typename T>
            using remove = type_list_remove_t<ThisL, T>;

            template<template<typename> typename Condition>
            using remove_if = type_list_remove_if_t<ThisL, Condition>;

            template<template<typename> typename Condition>
            using retain_if = type_list_retain_if_t<ThisL, Condition>;

            template<typename T>
            static constexpr auto count = type_list_count_v<ThisL, T>;

            template<template<typename> typename Condition>
            static constexpr auto count_if = type_list_count_if_v<ThisL, Condition>;

            template<template<typename> typename Condition>
            static constexpr auto all_match = type_list_all_match_v<ThisL, Condition>;

            template<template<typename> typename Condition>
            static constexpr auto none_match = type_list_none_match_v<ThisL, Condition>;

            template<template<typename> typename Condition>
            static constexpr auto any_match = type_list_any_match_v<ThisL, Condition>;

            template<type_list_detail::type_list_index_t I, typename T>
            using at_or = type_list_at_or_t<ThisL, I, T>;

            template<typename T>
            using front_or = at_or<0, T>;

            template<typename T>
            using back_or = at_or<size - 1, T>;

            template<template<typename> typename Func>
            using transform = type_list_transform_t<ThisL, Func>;

            using without_duplicates = type_list_without_duplicates_t<ThisL>;

            static auto constexpr has_duplicates = type_list_has_duplicates_v<ThisL>;

            template<typename T>
            static auto constexpr contains = type_list_contains_v<ThisL, T>;
        };
    }
    
    template<>
    struct type_list<> : public type_list_detail::type_list_base<type_list<>> {};

    template<typename... Ts> struct type_list : public type_list_detail::type_list_base<type_list<Ts...>> {
    private:
        using this_t = type_list;
        using base = type_list_detail::type_list_base<this_t>;

    public:
        // These are safe because we know we have arguments

        template<type_list_detail::type_list_index_t I>
        using at = type_list_detail::type_list_at_t<this_t, I>;

        using front = at<0>;

        using back = at<base::size - 1>;
    };
}

