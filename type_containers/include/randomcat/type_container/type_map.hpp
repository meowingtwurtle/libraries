#pragma once

#include <randomcat/type_container/type_list.hpp>

namespace randomcat::type_container {
    // Users are not permitted to introspect the template argument to type_map
    template<typename T>
    struct type_map;

    using empty_type_map = type_map<empty_type_list>;

    template<typename Key, typename Value>
    struct type_map_node {
        using key = Key;
        using value = Value;
    };
    
    namespace type_map_detail {
        template<template<typename> typename TT>
        struct wrap_s {
            template<typename T>
            struct value_for {
                using type = TT<T>;
            };
        };
        
        template<typename... Ts>
        inline constexpr auto invalid = false;

        template<typename T>
        struct is_type_map : std::false_type {};

        template<typename... Ts>
        struct is_type_map<type_map<Ts...>> : std::true_type {};

        template<typename T>
        inline auto constexpr is_type_map_v = is_type_map<T>::value;
        
        template<typename T>
        struct is_type_map_node : std::false_type {};

        template<typename... Ts>
        struct is_type_map_node<type_map_node<Ts...>> : std::true_type {};

        template<typename T>
        inline auto constexpr is_type_map_node_v = is_type_map_node<T>::value;

        template<typename T>
        struct node_info {
            static_assert(invalid<T> , "Argument must be a type_map_node");
        };

        template<typename K, typename V>
        struct node_info<type_map_node<K, V>> {
            using key = K;
            using value = V;
        };

        template<typename T>
        using node_value_t = typename node_info<T>::value;

        template<typename T>
        using node_key_t = typename node_info<T>::key;

        template<typename Target>
        struct node_has_key_s {
            template<typename Node>
            struct value_for_s {
                static auto constexpr value = std::is_same_v<Target, node_key_t<Node>>;
            };
        };

        template<typename T>
        struct require_type_map {
            static_assert(is_type_map_v<T>, "Argument must be a type_map");
            using type = T;
        };

        template<typename T>
        using require_type_map_t = typename require_type_map<T>::type;
        
        template<template<typename...> typename TT, typename... Ts>
        using eval_t = typename TT<Ts...>::type;
        
        template<typename M>
        struct type_map_nodes {
            static_assert(invalid<M>, "Argument must be a type_map");
        };
        
        template<typename NodeL>
        struct type_map_nodes<type_map<NodeL>> {
            using type = NodeL;
        };
        
        template<typename M>
        using type_map_nodes_t = typename type_map_nodes<M>::type;
        
        template<typename M>
        using type_map_keys_t = type_list_transform_t<type_map_nodes_t<M>, wrap_s<node_key_t>::template value_for>;
        
        template<template<typename, typename> typename KVFunc>
        struct wrap_kv_func {
            template<typename Node>
            using value_for = KVFunc<node_key_t<Node>, node_value_t<Node>>;
        };
    }
    
    template<typename T>
    inline auto constexpr is_type_map_v = type_map_detail::is_type_map_v<T>;

    template<typename M>
    using type_map_nodes_t = type_map_detail::type_map_nodes_t<type_map_detail::require_type_map_t<M>>;

    template<typename M, template<typename, typename> typename Condition>
    using type_map_retain_if_t = type_map<type_list_retain_if_t<type_map_nodes_t<M>, type_map_detail::wrap_kv_func<Condition>::template value_for>>;

    template<typename M, template<typename, typename> typename Condition>
    using type_map_remove_if_t = type_map<type_list_remove_if_t<type_map_nodes_t<M>, type_map_detail::wrap_kv_func<Condition>::template value_for>>;
    
    template<typename M, typename K>
    using type_map_remove_key_t = type_map_remove_if_t<M, type_map_detail::node_has_key_s<K>::template value_for_s>;

    template<typename M, typename K>
    using type_map_get_t = type_map_detail::node_value_t<type_list_at_t<type_list_retain_if_t<type_map_nodes_t<M>, type_map_detail::node_has_key_s<K>::template value_for_s>, 0>>;

    template<typename M, typename K, typename Default>
    using type_map_get_or_t = type_map_detail::node_value_t<type_list_at_or_t<type_list_retain_if_t<type_map_nodes_t<M>, type_map_detail::node_has_key_s<K>::template value_for_s>, 0, type_map_node<void, Default>>>;

    template<typename M, typename K, typename V>
    using type_map_set_t = type_map<type_list_push_front_t<type_list_remove_if_t<type_map_nodes_t<M>, type_map_detail::node_has_key_s<K>::template value_for_s>, type_map_node<K, V>>>;
    
    template<typename M, short _force_index = 0>
    using type_map_first_key_t = type_map_detail::node_key_t<type_list_at_t<type_map_nodes_t<M>, _force_index>>;
    
    template<typename M>
    using type_map_keys_t = type_list_transform_t<type_map_nodes_t<M>, type_map_detail::wrap_s<type_map_detail::node_key_t>::template value_for>;

    template<typename M>
    inline auto constexpr type_map_size_v = type_map_detail::require_type_map_t<M>::size;
    
    template<typename M>
    inline auto constexpr type_map_is_empty_v = type_map_size_v<M> == 0;
    
    template<typename M1, typename M2>
    inline auto constexpr type_maps_have_key_overlap_v = type_lists_have_matches_v<type_map_keys_t<M1>, type_map_keys_t<M2>>;
    
    template<typename M, template<typename, typename> typename Func>
    inline auto constexpr type_map_all_match_v = type_map_is_empty_v<type_map_remove_if_t<M, Func>>;
    
    namespace type_map_detail {
        template<typename M, typename K>
        struct type_map_has_key {
            static_assert(is_type_map_v<M>, "Argument must be a type_map");
            
        private:
            struct does_not_exist {};
            
        public:
            static auto constexpr value = !std::is_same_v<type_map_get_or_t<M, K, does_not_exist>, does_not_exist>;
        };
        
        template<typename M, typename K>
        inline auto constexpr type_map_has_key_v = type_map_has_key<M, K>::value;

        template<typename First, typename Second>
        inline auto constexpr type_map_equal_v = type_lists_are_equal_unordered_v<type_map_nodes_t<First>, type_map_nodes_t<Second>>;
        
        template<typename Map1, typename Map2, template<typename, typename> typename Merge, typename NullValue, typename Enable = void>
        struct type_map_merge {
            static_assert(is_type_map_v<Map1>, "Argument must be a type_map");
            static_assert(is_type_map_v<Map2>, "Argument must be a type_map");
            
            struct merge_node {
                template<typename Node>
                struct value_for {
                    using type = type_map_node<node_key_t<Node>, eval_t<Merge, node_value_t<Node>, type_map_get_or_t<Map2, node_key_t<Node>, NullValue>>>;
                };
            };
            
            template<typename M>
            struct has_key_wrap {
                template<typename K, typename V>
                struct value_for {
                    static auto constexpr value = type_map_has_key_v<M, K>;
                };
            };
            
            using one_way_merge = type_map<type_list_transform_t<type_map_nodes_t<Map1>, merge_node::template value_for>>;
            using second_no_keys_from_first = type_map_remove_if_t<Map2, has_key_wrap<Map1>::template value_for>;

            using type = typename type_map_merge<second_no_keys_from_first, one_way_merge, Merge, NullValue>::type;
        };
        
        template<typename Map1, typename Map2, template<typename, typename> typename Merge, typename NullValue>
        struct type_map_merge<Map1, Map2, Merge, NullValue, std::enable_if_t<!type_maps_have_key_overlap_v<Map1, Map2>>> {
            using type = type_map<type_list_concat_t<type_map_nodes_t<Map1>, type_map_nodes_t<Map2>>>;
        };
        
        template<typename M1, typename M2, template<typename, typename> typename Merge, typename NullValue>
        using type_map_merge_t = typename type_map_merge<M1, M2, Merge, NullValue>::type;
        
        template<typename M, template<typename, typename> typename ValueFunc>
        struct type_map_transform_value {
            static_assert(is_type_map_v<M>, "Argument must be a type_map");
            
            template<typename Node>
            struct do_transform {
                using type = type_map_node<node_key_t<Node>, eval_t<ValueFunc, node_key_t<Node>, node_value_t<Node>>>;
            };
            
            using type = type_map<type_list_transform_t<type_map_nodes_t<M>, do_transform>>;
        };
        
        template<typename M, template<typename, typename> typename ValueFunc>
        using type_map_transform_value_t = typename type_map_transform_value<M, ValueFunc>::type;
    }

    template<typename M1, typename M2>
    inline auto constexpr type_maps_are_equal_v = type_map_detail::type_map_equal_v<type_map_detail::require_type_map_t<M1>, type_map_detail::require_type_map_t<M2>>;

    template<typename M, typename K>
    inline auto constexpr type_map_has_key_v = type_map_detail::type_map_has_key_v<type_map_detail::require_type_map_t<M>, K>;
    
    template<typename M1, typename M2, template<typename, typename> typename Merge, typename NullValue>
    using type_map_merge_t = type_map_detail::type_map_merge_t<type_map_detail::require_type_map_t<M1>, type_map_detail::require_type_map_t<M2>, Merge, NullValue>;
    
    template<typename M, template<typename, typename> typename ValueFunc>
    using type_map_transform_value_t = type_map_detail::type_map_transform_value_t<type_map_detail::require_type_map_t<M>, ValueFunc>;
    
    template<typename NodeL>
    struct type_map {
    private:
        using nodes = NodeL;
        using this_t = type_map<NodeL>;
        
    public:
        
        static_assert(is_type_list_v<nodes>, "NodeL must be a type_list of type_map_node");
        static_assert(type_list_all_match_v<NodeL, type_map_detail::template is_type_map_node>, "NodeL must be a type_list of type_map_node");
        
        static constexpr auto size = nodes::size;
        
        template<typename Key, typename Value>
        using set = type_map_set_t<this_t, Key, Value>;
        
        template<typename Key>
        using get = type_map_get_t<this_t, Key>;
        
        template<typename Key, typename Default>
        using get_or = type_map_get_or_t<this_t, Key, Default>;
        
        template<template<typename, typename> typename Condition>
        using remove_if = type_map_remove_if_t<this_t, Condition>;

        template<template<typename, typename> typename Condition>
        using retain_if = type_map_retain_if_t<this_t, Condition>;

        template<typename Key>
        using remove_key = type_map_remove_key_t<this_t, Key>;
        
        template<typename OtherMap>
        static constexpr auto equal_to = type_maps_are_equal_v<this_t, OtherMap>;
        
        template<typename K>
        static auto constexpr has_key = type_map_has_key_v<this_t, K>;
    };
    
    namespace type_map_detail {
        template<typename... Ts>
        struct type_map_by_pair_s;
        
        template<typename T>
        struct type_map_by_pair_s<T> {
            static_assert(invalid<T>, "Argument number must be even");
        };
        
        template<>
        struct type_map_by_pair_s<> {
            using type = empty_type_map;
        };
        
        template<typename K0, typename V0, typename... Rest>
        struct type_map_by_pair_s<K0, V0, Rest...> {
            using type = type_map_set_t<typename type_map_by_pair_s<Rest...>::type, K0, V0>;
        };
        
        template<typename... Ts>
        using type_map_by_pair = typename type_map_by_pair_s<Ts...>::type;
    }
    
    template<typename... Ts>
    using type_map_by_pair_t = type_map_detail::type_map_by_pair<Ts...>;
}
