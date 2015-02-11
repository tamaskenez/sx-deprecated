#ifndef TRAITS_INCLUDED_204394
#define TRAITS_INCLUDED_204394

#include <type_traits>
#include <vector>

namespace sx {

    struct container_traits_tags {
        struct indexable {
        };
        struct use_const_index_iterator {
        };
        struct use_mutable_index_iterator {
        };
        struct use_const_pointer_iterator {
        };
        struct use_mutable_pointer_iterator {
        };
    };

    template<typename T>
    struct container_traits {
        static const bool indexable = std::is_base_of<container_traits_tags::indexable, T>::value;
        static const bool use_const_index_iterator = std::is_base_of<container_traits_tags::use_const_index_iterator, T>::value;
        static const bool use_mutable_index_iterator = std::is_base_of<container_traits_tags::use_mutable_index_iterator, T>::value;
        static const bool use_const_pointer_iterator = std::is_base_of<container_traits_tags::use_const_pointer_iterator, T>::value;
        static const bool use_mutable_pointer_iterator = std::is_base_of<container_traits_tags::use_mutable_pointer_iterator, T>::value;
    };

    template<typename T>
    struct container_traits<std::vector<T>> {
        static const bool indexable = true;
        static const bool use_const_index_iterator = false;
        static const bool use_mutable_index_iterator = false;
    };

    template<typename T>
    class darray1;

    template<typename T>
    struct container_traits<darray1<T>> {
        static const bool indexable = true;
        static const bool use_const_index_iterator = false;
        static const bool use_mutable_index_iterator = false;
    };

}

#endif

