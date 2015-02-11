#ifndef SX_INCLUDED_27402934
#define SX_INCLUDED_27402934

#include <cstdio>

#include "sx/array1.h"
#include "sx/array2.h"
#include "sx/index_iterator.h"
#include "sx/eager_ops.h"
#include "sx/proxy_iota.h"
#include "sx/stdabbrev.h"
#include "sx/stdaux.h"

namespace sx {
    template<typename T> using vec = array1<T>;
    template<typename T> using mvec = marray1<T>;
    template<typename T> using dvec = darray1<T>;

    template<typename T> using mat = array2<T>;
    template<typename T> using mmat = marray2<T>;
    template<typename T> using dmat = darray2<T>;


#define SX_DEF_POSTFIX(B, T, P) typedef B<T> B##_##P;
#define SX_DEF_ALL_POSTFIX(B)     \
    SX_DEF_POSTFIX(B, int, i)     \
    SX_DEF_POSTFIX(B, double, d)  \
    SX_DEF_POSTFIX(B, char, c)

    SX_DEF_ALL_POSTFIX(vec)
    SX_DEF_ALL_POSTFIX(mvec)
    SX_DEF_ALL_POSTFIX(dvec)
    SX_DEF_ALL_POSTFIX(mat)
    SX_DEF_ALL_POSTFIX(mmat)
    SX_DEF_ALL_POSTFIX(dmat)
    SX_DEF_ALL_POSTFIX(array1)
    SX_DEF_ALL_POSTFIX(marray1)
    SX_DEF_ALL_POSTFIX(darray1)
    SX_DEF_ALL_POSTFIX(array2)
    SX_DEF_ALL_POSTFIX(marray2)
    SX_DEF_ALL_POSTFIX(darray2)

#undef SX_DEF_POSTFIX
#undef SX_DEF_ALL_POSTFIX

//    struct size_op {
//        template<typename T>
//        decltype(std::declval<const T>().size()) operator*(const T &x) const {
//            return x.size();
//        }
//    };
//
//#define SIZE ::sx::size_op()*

}

#endif
