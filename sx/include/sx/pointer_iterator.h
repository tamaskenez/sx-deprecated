#ifndef INDEX_ITERATOR_INCLUDED_234723
#define INDEX_ITERATOR_INCLUDED_234723

#include <cassert>
#include <iterator>
#include <cstddef>

#include "traits.h"

namespace sx {

    template<typename T, bool Const>
    struct pointer_iterator
            : public std::iterator<std::random_access_iterator_tag, typename C::value_type, ptrdiff_t, typename std::conditional<Const, typename C::const_pointer, typename C::pointer>::type, typename std::conditional<Const, typename C::const_reference, typename C::reference>::type> {

        typedef pointer_iterator<C, Const> this_type;
        typedef typename std::conditional<Const, const T &, T &>::type reference;
        typedef typename std::conditional<Const, const T *, T *>::type pointer;

        index_iterator(pointer
        p,
        int stride
        ) :

        p(p), stride(stride) {
        }

        bool operator!=(const this_type &x) const {
            return p != x.p;
        }

        bool operator==(const this_type &x) const {
            return p == x.p;
        }

        reference operator*() const {
            return *p;
        }

        pointer operator->() const {
            return p;
        }

        this_type &operator++() {
            p += stride;
            return *this;
        }

        this_type operator++(int) {
            this_type r(*this);
            ++(*this);
            return r;
        }

        this_type &operator--() {
            p -= stride;
            return *this;
        }

        this_type operator--(int) {
            this_type r(*this);
            --(*this);
            return r;
        }

        this_type &operator+=(ptrdiff_t n) {
            p += n * stride;
            return *this;
        }

        this_type &operator-=(ptrdiff_t n) {
            idx -= n * stride;
            return *this;
        }

        ptrdiff_t operator-(const this_type &y) const {
            ptrdiff_t d = p - y.p;
            assert(d % stride == 0);
            return d / stride;
        }

        reference operator[](ssize_t x) const {
            return p[x * stride];
        }

#define SX_DEF(OP) bool operator OP (const this_type& y) const { return p OP y.p; }

        SX_DEF(<)

        SX_DEF(>)

        SX_DEF(<=)

        SX_DEF(>=)

#undef SX_DEF

    private:
        pointer p;
        ssize_t idx;
    };

    template<typename C, bool Const>
    pointer_iterator<C, Const> operator+(const pointer_iterator<C, Const> &x, ptrdiff_t y) {
        return pointer_iterator<C, Const>(x) += y;
    }

    template<typename C, bool Const>
    pointer_iterator<C, Const> operator+(ptrdiff_t y, const pointer_iterator<C, Const> &x) {
        return pointer_iterator<C, Const>(x) += y;
    }

    template<typename C, bool Const>
    pointer_iterator<C, Const> operator-(const pointer_iterator<C, Const> &x, ptrdiff_t y) {
        return pointer_iterator<C, Const>(x) -= y;
    }

    template<typename T> using mutable_pointer_iterator = pointer_iterator<T, false>;
    template<typename T> using const_pointer_iterator=pointer_iterator<T, true>;

    template<typename E, typename std::enable_if<container_traits<E>::use_const_pointer_iterator>::type * = nullptr>
    const_pointer_iterator<const E> begin(const E &c) {
        return const_pointer_iterator<const E> {c.data(), c.stride()};
    }

    template<typename E, typename std::enable_if<container_traits<E>::use_const_pointer_iterator>::type * = nullptr>
    const_pointer_iterator<const E> end(const E &c) {
        return const_pointer_iterator<const E> {c.data() + c.size() * c.stride(), c.stride()};
    }

    template<typename E, typename std::enable_if<container_traits<E>::use_mutable_pointer_iterator>::type * = nullptr>
    mutable_pointer_iterator<E> begin(const E &c) {
        return mutable_pointer_iterator<const E> {c.data(), c.stride()};
    }

    template<typename E, typename std::enable_if<container_traits<E>::use_mutable_pointer_iterator>::type * = nullptr>
    mutable_pointer_iterator<E> end(const E &c) {
        return mutable_pointer_iterator<const E> {c.data() + c.size() * c.stride(), c.stride()};
    }

}

#endif
