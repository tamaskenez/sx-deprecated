#ifndef STDAUX_INCLUDED_2043923954
#define STDAUX_INCLUDED_2043923954

#include "macros.h"

namespace sx {

    template<typename T>
    void unique_trunc(T &c) {
        c.erase(std::unique(c.begin(), c.end()), c.end());
    }

    template<typename T, typename BinaryPredicate>
    void unique_trunc(T &c, BinaryPredicate pred) {
        c.erase(std::unique(c.begin(), c.end(), pred), c.end());
    }

    template<typename T>
    T set_difference(const T &x, const T &y) {
        T result(x.size());
        result.erase(
                std::set_difference(BEGINEND(x), BEGINEND(y), result.begin()),
                result.end());
        return result;
    }

    template<typename T>
    T set_intersection(const T &x, const T &y) {
        T result(std::min(x.size(), y.size()));
        result.erase(
                std::set_intersection(BEGINEND(x), BEGINEND(y), result.begin()),
                result.end());
        return result;
    }

    template<typename T>
    typename T::reference push_back_default(T &x) {
        x.emplace_back();
        return x.back();
    }

    template<typename T, typename U>
    void insert_at_end(T &t, const U &u) {
        t.insert(t.end(), BEGINEND(u));
    }
}

#endif

