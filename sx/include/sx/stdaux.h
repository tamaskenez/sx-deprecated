#ifndef STDAUX_INCLUDED_2043923954
#define STDAUX_INCLUDED_2043923954

namespace sx {

    template<typename T>
    void unique_trunc(T &c) {
        c.erase(std::unique(c.begin(), c.end()), c.end());
    }

    template<typename T, typename BinaryPredicate>
    void unique_trunc(T &c, BinaryPredicate pred) {
        c.erase(std::unique(c.begin(), c.end(), pred), c.end());
    }
}

#endif

