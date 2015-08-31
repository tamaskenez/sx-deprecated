#ifndef PROXY_IOTA_INCLUDED_2398439
#define PROXY_IOTA_INCLUDED_2398439

#include <cassert>

#include "array1.h"
#include "index_iterator.h"

namespace sx {
    template<typename T>
    class array1_proxy_iota
            : public container_traits_tags::indexable, public container_traits_tags::use_const_index_iterator {
    public:
        typedef T value_type;
        typedef T reference;
        typedef T const_reference;
        typedef const T *pointer;
        typedef const T *const_pointer;

        class iterator
                : public std::iterator<std::random_access_iterator_tag, T, ptrdiff_t, pointer, const_pointer> {
        public:
            typedef iterator this_type;
            typedef T reference;

            iterator(const T &idx) : idx(idx) {
            }

            bool operator==(const iterator &x) const {
                return idx == x.idx;
            }

            bool operator!=(const iterator &x) const {
                return idx != x.idx;
            }

            reference operator*() const {
                return idx;
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

            ptrdiff_t operator-(const const_index_iterator<T> &y) const {
                return idx - y.idx;
            }

            reference operator[](ptrdiff_t x) const {
                return idx + x;
            }

#define SX_DEF(OP) bool operator OP (const this_type& y) const { return idx OP y.idx; }

            SX_DEF(<)

            SX_DEF(>)

            SX_DEF(<=)

            SX_DEF(>=)

#undef SX_DEF
        private:
            T idx;
        };

        explicit array1_proxy_iota(const T &endidx) : startidx(0), endidx(endidx) {
        }

        array1_proxy_iota(const T &startidx, const T &endidx) : startidx(startidx), endidx(endidx) {
        }

        iterator begin() const {
            return iterator(startidx);
        }

        iterator end() const {
            return iterator(endidx);
        }

        const_reference operator[](ptrdiff_t idx) const {
            assert(0 <= idx && idx < endidx);
            return (T) idx;
        }

        ptrdiff_t size() const {
            return endidx;
        }

    private:
        const T startidx, endidx;
    };

    template<typename T>
    array1_proxy_iota<T> iota(const T &endIdx) {
        return array1_proxy_iota<T>(endIdx);
    }

    template<typename T, typename T2>
    array1_proxy_iota<T> iota(const T &startIdx, const T2 &endIdx) {
        return array1_proxy_iota<T>(startIdx, endIdx);
    }

    template<typename T>
    typename array1_proxy_iota<T>::iterator operator+(const typename array1_proxy_iota<T>::iterator &x, ptrdiff_t y) {
        return array1_proxy_iota<T>::iterator(x) += y;
    }

    template<typename T>
    typename array1_proxy_iota<T>::iterator operator+(ptrdiff_t y, const typename array1_proxy_iota<T>::iterator &x) {
        return array1_proxy_iota<T>::iterator(x) += y;
    }

    struct iota_op {
        template<typename T>
        array1_proxy_iota<T> operator*(const T &x) const {
            return iota(x);
        }
    };

#define IOTA iota_op()*
}


#endif

