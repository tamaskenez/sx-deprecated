#ifndef PROXY_INDEX_AT_INCLUDED_247983
#define PROXY_INDEX_AT_INCLUDED_247983

#include "traits.h"

namespace sx {
    template<typename C1, typename C2>
    class proxy_const_indexable_at_indexable
            : public container_traits_tags::indexable, public container_traits_tags::use_const_index_iterator {
    public:
        typedef proxy_const_indexable_at_indexable<C1, C2> this_type;

        proxy_const_indexable_at_indexable(const C1 &c1, const C2 &c2) : c1(c1), c2(c2) {
        }

        proxy_const_indexable_at_indexable(const this_type &) = delete;

        proxy_const_indexable_at_indexable(this_type &&x) : c1(x.c1), c2(x.c2) {
        }

        void operator=(const this_type &) = delete;

        ssize_t size() const {
            return c2.size();
        }

        typename C1::const_reference operator[](typename C2::size_type x) const {
            return c1[c2[x]];
        }

    private:
        const C1 &c1;
        const C2 &c2;
    };

    //enable if both indexable and C2::value_type is integer
    template<typename C1, typename C2, typename std::enable_if<
            container_traits<C1>::indexable &&
                    container_traits<C2>::indexable &&
                    std::is_integral<typename C2::value_type>::value
    >::type * = nullptr>
    proxy_const_indexable_at_indexable<C1, C2> index_at(const C1 &c1, const C2 &c2) {
        return proxy_const_indexable_at_indexable<C1, C2>(c1, c2);
    }
}

#endif
