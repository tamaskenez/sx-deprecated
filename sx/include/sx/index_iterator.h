#ifndef INDEX_ITERATOR_INCLUDED_234723
#define INDEX_ITERATOR_INCLUDED_234723

#include <cassert>
#include <iterator>
#include <cstddef>

#include "traits.h"

namespace sx {

    template<typename C, bool Const>
    struct index_iterator
            : public std::iterator<std::random_access_iterator_tag, typename C::value_type, ptrdiff_t, typename std::conditional<Const, typename C::const_pointer, typename C::pointer>::type, typename std::conditional<Const, typename C::const_reference, typename C::reference>::type> {

        typedef index_iterator<C, Const> this_type;
        typedef typename std::conditional<Const, typename C::const_reference, typename C::reference>::type reference;
        typedef typename std::conditional<Const, typename C::const_pointer, typename C::pointer>::type pointer;

        index_iterator(C *that, ptrdiff_t idx) : that(that), idx(idx) {
        }

        bool operator!=(const this_type &x) const {
            assert(that == x.that);
            return idx != x.idx;
        }

        bool operator==(const this_type &x) const {
            assert(that == x.that);
            return idx == x.idx;
        }

        reference operator*() const {
            return (*that)[idx];
        }

        pointer operator->() const {
            return &((*that)[idx]);
        }

        this_type &operator++() {
            ++idx;
            return *this;
        }

        this_type operator++(int) {
            this_type r(*this);
            ++(*this);
            return r;
        }

        this_type &operator--() {
            --idx;
            return *this;
        }

        this_type operator--(int) {
            this_type r(*this);
            --(*this);
            return r;
        }

        this_type &operator+=(ptrdiff_t n) {
            idx += n;
            return *this;
        }

        this_type &operator-=(ptrdiff_t n) {
            idx -= n;
            return *this;
        }

        ptrdiff_t operator-(const this_type &y) const {
            assert(that == y.that);
            return idx - y.idx;
        }

        reference operator[](ptrdiff_t x) const {
            return (*that)[idx + x];
        }

#define SX_DEF(OP) bool operator OP (const this_type& y) const { assert(that == y.that); return idx OP y.idx; }

        SX_DEF(<)

        SX_DEF(>)

        SX_DEF(<=)

        SX_DEF(>=)

#undef SX_DEF

    private:
        C *that;
        ptrdiff_t idx;
    };

    template<typename C, bool Const>
    index_iterator<C, Const> operator+(const index_iterator<C, Const> &x, ptrdiff_t y) {
        return index_iterator<C, Const>(x) += y;
    }

    template<typename C, bool Const>
    index_iterator<C, Const> operator+(ptrdiff_t y, const index_iterator<C, Const> &x) {
        return index_iterator<C, Const>(x) += y;
    }

    template<typename C, bool Const>
    index_iterator<C, Const> operator-(const index_iterator<C, Const> &x, ptrdiff_t y) {
        return index_iterator<C, Const>(x) -= y;
    }

    template<typename T> using mutable_index_iterator = index_iterator<T, false>;
    template<typename T> using const_index_iterator=index_iterator<T, true>;

    template<typename E, typename std::enable_if<container_traits<E>::use_const_index_iterator>::type * = nullptr>
    const_index_iterator<const E> begin(const E &c) {
        return const_index_iterator<const E> {&c, 0};
    }

    template<typename E, typename std::enable_if<container_traits<E>::use_const_index_iterator>::type * = nullptr>
    const_index_iterator<const E> end(const E &c) {
        return const_index_iterator<const E> {&c, c.size()};
    }

    template<typename E, typename std::enable_if<container_traits<E>::use_mutable_index_iterator>::type * = nullptr>
    mutable_index_iterator<E> begin(const E &c) {
        return mutable_index_iterator<const E> {&c, 0};
    }

    template<typename E, typename std::enable_if<container_traits<E>::use_mutable_index_iterator>::type * = nullptr>
    mutable_index_iterator<E> end(const E &c) {
        return mutable_index_iterator<const E> {&c, c.size()};
    }

}

#endif
