#ifndef EAGER_OPS_INCLUDED_014023
#define EAGER_OPS_INCLUDED_014023

#include "array1.h"
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include "macros.h"
#include "sx/proxy_iota.h"
#include "array1.h"

namespace sx {
    // where
    template<typename E, typename std::enable_if<container_traits<E>::indexable>::type * = nullptr>
    darray1<ssize_t> where(const E &e) {
        const ssize_t N = e.size();
        ssize_t count = 0;
        for (ssize_t i = 0; i < N; ++i)
            if (e[i])
                ++count;
        darray1<ssize_t> result;
        result.reserve(count);
        for (ssize_t i = 0; i < N; ++i)
            if (e[i])
                result.push_back(i);
        return result;
    }

    // op==(list, list)
    template<typename E1, typename E2, typename std::enable_if<container_traits<E1>::indexable && container_traits<E2>::indexable>::type * = nullptr>
    bool operator==(const E1 &e1, const E2 &e2) {
        const ssize_t N = e1.size();
        if (N != e2.size())return false;

        for (auto i: IOTA N) if (e1[i] != e2[i]) return false;

        return true;
    }

    // op+(list, list)
    template<typename E1, typename E2, typename std::enable_if<container_traits<E1>::indexable && container_traits<E2>::indexable>::type * = nullptr>
    darray1<decltype(std::declval<typename E1::value_type>() + std::declval<typename E1::value_type>())> operator+(const E1 &e1, const E2 &e2) {
        const ssize_t N = e1.size();
        if (N != e2.size()) throw std::runtime_error("op+(list,list) different sizes");
        darray1<decltype(std::declval<typename E1::value_type>() + std::declval<typename E1::value_type>())> r(N);

        for (auto i : IOTA N) r[i] = e1[i] + e2[i];

        return r;
    }

    // op*(list, atom)
    template<typename E1, typename T2, typename std::enable_if<container_traits<E1>::indexable && !container_traits<T2>::indexable>::type * = nullptr>
    darray1<decltype(std::declval<typename E1::value_type>() * std::declval<T2>())> operator*(const E1 &e1, const T2 &t2) {
        const ssize_t N = e1.size();

        darray1<decltype(std::declval<typename E1::value_type>() * std::declval<T2>())> r(N);

        for (auto i : IOTA N) r[i] = e1[i] * t2;

        return r;
    }

    // op/(list, atom)
    template<typename E1, typename T2, typename std::enable_if<container_traits<E1>::indexable && !container_traits<T2>::indexable>::type * = nullptr>
    darray1<decltype(std::declval<typename E1::value_type>() / std::declval<T2>())> operator/(const E1 &e1, const T2 &t2) {
        const ssize_t N = e1.size();

        darray1<decltype(std::declval<typename E1::value_type>() / std::declval<T2>())> r(N);

        for (auto i : IOTA N) r[i] = e1[i] / t2;

        return r;
    }

    // op==(list, atom)
    template<typename E1, typename T2, typename std::enable_if<container_traits<E1>::indexable && !container_traits<T2>::indexable>::type * = nullptr>
    darray1<bool> operator==(const E1 &e1, const T2 &t2) {
        const ssize_t N = e1.size();

        darray1<bool> result(N);
        for (ssize_t i = 0; i < N; ++i)
            result[i] = e1[i] == t2;

        return result;
    }

    // op==(atom, list)
    template<typename E1, typename T2, typename std::enable_if<container_traits<E1>::indexable && !container_traits<T2>::indexable>::type * = nullptr>
    darray1<bool> operator==(const T2 &t2, const E1 &e1) {
        return e1 == t2;
    }

    // horzcat(list, atom)
    template<typename E, typename std::enable_if<container_traits<E>::indexable>::type * = nullptr>
    darray1<typename E::value_type> horzcat(const E &e, const typename E::value_type &t) {
        darray1<typename E::value_type> result(BEGINEND(e));
        result.push_back(t);
    }

    // horzcat(atom, list)
    template<typename E, typename std::enable_if<container_traits<E>::indexable>::type * = nullptr>
    darray1<typename E::value_type> horzcat(const typename E::value_type &t, const E &e) {
        darray1<typename E::value_type> result;
        result.reserve(e.size() + 1);
        result.push_back(t);
        result.push_back(BEGINEND(e));
        return result;
    }

    // grade_down(list)
    template<typename E, typename std::enable_if<container_traits<E>::indexable>::type * = nullptr>
    darray1<typename E::size_type> grade_down(const E &e) {
        typedef typename E::size_type e_size_type;
        darray1<typename E::size_type> result;
        result.reserve(e.size());
        for (auto i:iota(e.size()))
            result.push_back(i);
        std::sort(BEGINEND(result), [&e](e_size_type x, e_size_type y) {
            return e[x] > e[y];
        });
        return result;
    }

    // grade_down(list)
    template<typename E, typename std::enable_if<container_traits<E>::indexable>::type * = nullptr>
    darray1<typename E::size_type> grade_up(const E &e) {
        typedef typename E::size_type e_size_type;
        darray1<typename E::size_type> result;
        result.reserve(e.size());
        for (auto i : iota(e.size()))
            result.push_back(i);
        std::sort(BEGINEND(result), [&e](e_size_type x, e_size_type y) {
            return e[x] < e[y];
        });
        return result;
    }

    // function object for drop(n, list) with n bound
    struct drop_bound1st {
        drop_bound1st(ssize_t x) : x(x) {
        }

        template<typename E, typename std::enable_if<container_traits<E>::indexable>::type * = nullptr>
        array1<typename E::value_type> operator()(const E &e) const {
            return x >= 0 ? e.slice(x, from_end(0)) : e.slice(0, from_end(x));
        }

    private:
        ssize_t x;
    };

    // return function object drop(n, list) with n bound
    drop_bound1st drop(ssize_t x) {
        return drop_bound1st(x);
    }

    // cut(idxlist, list)
    template<typename E1, typename E2, typename std::enable_if<container_traits<E1>::indexable && container_traits<E2>::indexable>::type * = nullptr>
    sx::darray1<sx::array1<typename E2::value_type>> cut(const E1 &idcs, const E2 &y) {
        darray1<array1<typename E2::value_type>> result;
        const ssize_t N = idcs.size();
        result.reserve(N);
        for (ssize_t i = 0; i < N; ++i) {
            const ssize_t lo = idcs[i];
            const ssize_t hi = i + 1 == N ? y.size() : idcs[i + 1];
            result.push_back(y.slice(lo, hi));
        }
        return result;
    }

    // each(Fx, list)
    template<typename UnaryPr, typename E, typename std::enable_if<container_traits<E>::indexable>::type * = nullptr>
    darray1<typename std::result_of<UnaryPr(typename E::const_reference)>::type> each(UnaryPr &&fun, const E &x) {
        const ssize_t N = x.size();
        darray1<typename std::result_of<UnaryPr(typename E::const_reference)>::type> result;
        result.reserve(N);
        for (ssize_t i = 0; i < N; ++i)
            result.push_back(fun(x[i]));
        return result;
    }

    // over(atom, Fxy, list)
    template<typename X, typename Fxy, typename V>
    X over(const X &x0, const Fxy &&f, const V &v) {
        auto result = x0;
        for (auto &&x: v) {
            result = f(result, x);
        }
        return result;
    }

    // over(Fxy, list)
    template<typename Fxy, typename V>
    int over(const Fxy &&f, const V &v) {
        if (v.size() == 0) {
            throw std::runtime_error("over: input cannot be empty");
        } else {
            auto it = begin(v);
            auto result = *it;
            ++it;
            for (; it != end(v); ++it) {
                result = f(result, *it);
            }
            return result;
        }
    }

    // sum(list)
    template<typename V>
    typename V::value_type sum(const V &v) {
        return std::accumulate(BEGINEND(v), typename V::value_type(0));
    }

    // prod(list)
    template<typename V>
    typename V::value_type prod(const V &v) {
        return std::accumulate(BEGINEND(v), typename V::value_type(1));
    }

    //min list
    template<typename V>
    typename V::const_reference min(const V &v) {
        return *std::min_element(BEGINEND(v));
    }

    //max list
    template<typename V>
    typename V::const_reference max(const V &v) {
        return *std::max_element(BEGINEND(v));
    }


    template<typename E, bool Const>
    class at_indexable_t {
    public:
        typedef typename std::conditional<Const, const E &, E &>::type range_reference_type;
        typedef at_indexable_t<E, Const> this_type;

        at_indexable_t(range_reference_type e) : e(e) {
        }

        //op=(atom)
        template<typename T, typename std::enable_if<!container_traits<T>::indexable>::type * = nullptr>
        const this_type &operator=(const T &t) const {
            for (auto i : iota(e.size())) e[i] = t;
            return *this;
        }

        //op=(indexable)
        template<typename T, typename std::enable_if<container_traits<T>::indexable>::type * = nullptr>
        const this_type &operator=(const T &x) const {
            if (e.size() != x.size()) throw std::runtime_error("at::op= different sizes");
            for (auto i : iota(e.size())) e[i] = x[i];
            return *this;
        }

    private:
        range_reference_type e;
    };

    //at(list) = ...
    template<typename E1, typename std::enable_if<container_traits<E1>::indexable>::type * = nullptr>
    at_indexable_t<E1, false> at(E1 &e1) {
        return at_indexable_t<E1, false>(e1);
    }

    template<typename E1, typename std::enable_if<container_traits<E1>::indexable>::type * = nullptr>
    at_indexable_t<E1, true> at(const E1 &e1) {
        return at_indexable_t<E1, true>(e1);
    }

//    void f(mvec_d x){
//        dvec_d a = c.row(1); //must copy by value
//        mvec_d m = c.row(1); //what???
//        vec_d b = c.row(1); //must acquire immutable reference
//    }

    //for now only norm(vec, 2) is implemented
    //should accept only iterable, one dimensional containers (vector<vector<>> is okay, dmat is not)
    template<typename C>
    typename C::value_type norm(const C &v) {
        typename C::value_type sum = 0;
        for (auto &x:v) sum += x * x;
        return sqrt(sum);
    }

}
#endif
