#ifndef STDAUX_INCLUDED_20487234
#define STDAUX_INCLUDED_20487234

#include <algorithm>

namespace sx {
    template<typename T>
    void sort(T &c) {
        ::std::sort(c.begin(), c.end());
    }

    template<typename T, typename Pr>
    void sort(T &c, Pr &&pr) {
        ::std::sort(c.begin(), c.end(), pr);
    }

    template<typename T>
    typename T::iterator unique(T &c) {
        return ::std::unique(c.begin(), c.end());
    }

    template<typename T, typename BinaryPredicate>
    typename T::iterator unique(T &c, BinaryPredicate &&pred) {
        return ::std::unique(c.begin(), c.end(), pred);
    }

    template<typename T>
    void unique_trunc(T &c) {
        c.erase(unique(c), c.end());
    }

    template<typename T, typename BinaryPredicate>
    void unique_trunc(T &c, BinaryPredicate &&pred) {
        c.erase(unique(c, pred), c.end());
    }

    template<typename C>
    typename C::reference push_back_default(C &c) {
        c.push_back(typename C::value_type());
        return c.back();
    }

}

#endif
