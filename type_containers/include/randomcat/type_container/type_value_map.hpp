#pragma once

#include <randomcat/type_container/type_map.hpp>

namespace randomcat::type_container {
    // Users are not permitted to introspect the template arguments of type_value_map
    template<typename ValueType, typename Underlying>
    struct type_value_map;
    
    template<typename ValueType>
    using empty_type_value_map = type_value_map<ValueType, empty_type_map>;
    
    namespace type_value_map_detail {
        template<typename ValueType, ValueType Value>
        struct value_holder {
            using value_type = ValueType;
            static constexpr ValueType value = Value;
        };
        
        template<typename... Ts>
        inline constexpr bool invalid_v = false;
        
        template<typename M>
        struct type_value_map_info {
            static_assert(invalid_v<M>, "Argument must be a type_value_map");
        };
        
        template<typename ValueType, typename Underlying>
        struct type_value_map_info<type_value_map<ValueType, Underlying>> {
            using value_type = ValueType;
            using underlying = Underlying;
        };
        
        template<typename M>
        using type_value_map_underlying_t = typename type_value_map_info<M>::underlying;
        
        template<typename M>
        using type_value_map_value_type_t = typename type_value_map_info<M>::value_type;
        
        template<typename T>
        struct is_type_value_map   : std::false_type {};
        
        template<typename... Args>
        struct is_type_value_map<type_value_map<Args...>> : std::true_type {};
        
        template<typename T>
        inline auto constexpr is_type_value_map_v = is_type_value_map<T>::value;
        
        template<typename T>
        struct require_type_value_map {
            static_assert(is_type_value_map_v<T>);
            using type = T;
        };
        
        template<typename T>
        using require_type_value_map_t = typename require_type_value_map<T>::type;
        
        template<typename N>
        struct value_holder_info {
            static_assert(invalid_v<N>, "Argument must be a type_value_node");
        };
        
        template<typename ValueType, ValueType Value>
        struct value_holder_info<value_holder<ValueType, Value>> {
            using value_type = ValueType;
            static constexpr ValueType value = Value;
        };
        
        template<typename N>
        using value_holder_value_type_t = typename value_holder_info<N>::value_type;

        template<typename N>
        inline value_holder_value_type_t<N> constexpr value_holder_value_v = value_holder_info<N>::value;
        
        template<typename M, typename K>
        inline type_value_map_value_type_t<M> constexpr type_value_map_get_v = value_holder_value_v<type_map_get_t<type_value_map_underlying_t<M>, K>>;

        template<typename M, typename K, type_value_map_value_type_t<M> Default>
        inline type_value_map_value_type_t<M> constexpr type_value_map_get_or_v = value_holder_value_v<type_map_get_or_t<type_value_map_underlying_t<M>, K, value_holder<type_value_map_value_type_t<M>, Default>>>;
        
        template<typename ValueType, template<typename, ValueType> typename Function>
        struct wrap_kv_function_to_type_s {
            template<typename Key, typename WrappedValue>
            struct value_for {
                using type = value_holder<ValueType, Function<Key, value_holder_value_v<WrappedValue>>::value>;
            };
        };
        
        template<typename ValueType, template<ValueType, ValueType> typename Function>
        struct wrap_vv_function_to_type_s {
            template<typename WrappedValue0, typename WrappedValue1>    
            struct value_for {
                using type = value_holder<ValueType, Function<value_holder_value_v<WrappedValue0>, value_holder_value_v<WrappedValue1>>::value>;
            };
        };

        template<typename ValueType, template<typename, ValueType> typename Function>
        struct wrap_kv_function_to_value_s {
            template<typename Key, typename WrappedValue>
            struct value_for {
                static auto constexpr value = Function<Key, value_holder_value_v<WrappedValue>>::value;
            };
        };

        template<typename M, typename K, type_value_map_value_type_t<M> V>
        using type_value_map_set_t = type_value_map<type_value_map_value_type_t<M>, type_map_set_t<type_value_map_underlying_t<M>, K, value_holder<type_value_map_value_type_t<M>, V>>>;
        
        template<typename M, typename K>
        inline auto constexpr type_value_map_has_key_v = type_map_has_key_v<type_value_map_underlying_t<M>, K>;
        
        template<typename M0, typename M1>
        struct type_value_maps_are_equal_s {
            static_assert(std::is_same_v<type_value_map_value_type_t<M0>, type_value_map_value_type_t<M1>>, "Arguments must have the same underlying type");
            
            static auto constexpr value = type_maps_are_equal_v<type_value_map_underlying_t<M0>, type_value_map_underlying_t<M1>>;
        };
        
        template<typename M0, typename M1>
        inline auto constexpr type_value_maps_are_equal_v = type_value_maps_are_equal_s<M0, M1>::value;

        template<typename M, template<typename, type_value_map_value_type_t<M>> typename ValueFunc>
        using type_value_map_transform_value_t = type_value_map<type_value_map_value_type_t<M>, type_map_transform_value_t<type_value_map_underlying_t<M>, wrap_kv_function_to_type_s<type_value_map_value_type_t<M>, ValueFunc>::template value_for>>;

        template<typename M1, typename M2, template<type_value_map_value_type_t<M1>, type_value_map_value_type_t<M1>> typename Merge, type_value_map_value_type_t<M1> NullValue>
        struct type_value_map_merge_s {
            static_assert(is_type_value_map_v<M1>, "Argument must be a type_value_map");
            static_assert(is_type_value_map_v<M2>, "Argument must be a type_value_map");
            static_assert(std::is_same_v<type_value_map_value_type_t<M1>, type_value_map_value_type_t<M2>>, "Arguments must have the same value_type");
            
            using value_type = type_value_map_value_type_t<M1>;
            using type = type_value_map<value_type, type_map_merge_t<type_value_map_underlying_t<M1>, type_value_map_underlying_t<M2>, wrap_vv_function_to_type_s<value_type, Merge>::template value_for, value_holder<value_type, NullValue>>>;
        };

        template<typename M1, typename M2, template<type_value_map_value_type_t<M1>, type_value_map_value_type_t<M1>> typename Merge, type_value_map_value_type_t<M1> NullValue>
        using type_value_map_merge_t = typename type_value_map_merge_s<M1, M2, Merge, NullValue>::type;
    }
    
    template<typename M>
    using type_value_map_value_type_t = type_value_map_detail::type_value_map_value_type_t<M>;
    
    template<typename M, typename K, type_value_map_value_type_t<M> V>
    using type_value_map_set_t = type_value_map_detail::type_value_map_set_t<M, K, V>;
    
    template<typename M, typename K>
    inline constexpr auto type_value_map_get_v = type_value_map_detail::type_value_map_get_v<M, K>;
    
    template<typename M, typename K, type_value_map_value_type_t<M> Default>
    inline constexpr auto type_value_map_get_or_v = type_value_map_detail::type_value_map_get_or_v<M, K, Default>;
    
    template<typename M, typename K>
    inline constexpr auto type_value_map_has_key_v = type_value_map_detail::type_value_map_has_key_v<M, K>;
    
    template<typename M0, typename M1>
    inline constexpr auto type_value_maps_are_equal_v = type_value_map_detail::type_value_maps_are_equal_v<M0, M1>;
    
    template<typename M, template<typename, type_value_map_value_type_t<M>> typename Function>
    using type_value_map_retain_if_t = type_value_map<type_value_map_value_type_t<M>, type_map_retain_if_t<type_value_map_detail::type_value_map_underlying_t<M>, type_value_map_detail::wrap_kv_function_to_value_s<type_value_map_value_type_t<M>, Function>::template value_for>>;

    template<typename M, template<typename, type_value_map_value_type_t<M>> typename Function>
    using type_value_map_remove_if_t = type_value_map<type_value_map_value_type_t<M>, type_map_remove_if_t<type_value_map_detail::type_value_map_underlying_t<M>, type_value_map_detail::wrap_kv_function_to_value_s<type_value_map_value_type_t<M>, Function>::template value_for>>;
    
    template<typename M, typename K>
    using type_value_map_remove_key_t = type_value_map<type_value_map_value_type_t<M>, type_map_remove_key_t<type_value_map_detail::type_value_map_underlying_t<M>, K>>;
    
    template<typename M, template<typename, type_value_map_value_type_t<M>> typename ValueFunc>
    using type_value_map_transform_value_t = type_value_map_detail::type_value_map_transform_value_t<type_value_map_detail::require_type_value_map_t<M>, ValueFunc>;
    
    template<typename M1, typename M2, template<type_value_map_value_type_t<M1>, type_value_map_value_type_t<M1>> typename Merge, type_value_map_value_type_t<M1> NullValue>
    using type_value_map_merge_t = type_value_map_detail::type_value_map_merge_t<type_value_map_detail::require_type_value_map_t<M1>, type_value_map_detail::require_type_value_map_t<M2>, Merge, NullValue>;
    
    template<typename M>
    inline auto constexpr type_value_map_size_v = type_map_size_v<type_value_map_detail::type_value_map_underlying_t<M>>;
    
    template<typename M>
    inline auto constexpr type_value_map_is_empty_v = type_map_is_empty_v<type_value_map_detail::type_value_map_underlying_t<M>>;
    
    template<typename M, template<typename, type_value_map_value_type_t<M>> typename Func>
    inline auto constexpr type_value_map_all_match_v = type_value_map_is_empty_v<type_value_map_remove_if_t<M, Func>>;
    
    template<typename ValueType, typename UnderlyingMap>
    struct type_value_map {
        using value_type = ValueType;
        using this_t = type_value_map;
        
        template<typename K>
        static constexpr value_type get = type_value_map_get_v<this_t, K>;
        
        template<typename K, value_type Default>
        static constexpr value_type get_or = type_value_map_get_or_v<this_t, K, Default>;
        
        template<typename K, value_type V>
        using set = type_value_map_set_t<this_t, K, V>;
        
        template<typename K>
        using remove_key = type_value_map_remove_key_t<this_t, K>;
        
        template<template<typename, value_type> typename Function>
        using remove_if = type_value_map_remove_if_t<this_t, Function>;

        template<template<typename, value_type> typename Function>
        using retain_if = type_value_map_retain_if_t<this_t, Function>;
        
        template<typename K>
        static auto constexpr has_key = type_value_map_has_key_v<this_t, K>;
        
        template<typename OtherMap>
        static auto constexpr equal_to = type_value_maps_are_equal_v<this_t, OtherMap>;
    };
}
