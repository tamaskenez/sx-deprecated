#ifndef STDABBREV_INCLUDED_0827402983
#define STDABBREV_INCLUDED_0827402983

#include <tuple>
#include <array>
#include <vector>
#include <string>
#include <utility>
#include <initializer_list>
#include <limits>
#include <memory>

namespace sx {

    namespace stdabbrev {

        typedef std::vector<double> vector_d;
        typedef std::vector<int> vector_i;
        typedef std::vector<std::string> vector_s;
        typedef std::vector<bool> vector_b;

        using std::string;
        using std::tie;
        using std::move;
        using std::tuple;
        using std::make_tuple;
        using std::pair;
        using std::initializer_list;
        using std::make_pair;
        using std::vector;
        using std::array;
        using std::max;
        using std::min;

        template<size_t N> using array_i = std::array<int, N>;
        template<size_t N> using array_d = std::array<double, N>;
        template<typename T> using ilist = std::initializer_list<T>;
        template<typename T> using uptr = std::unique_ptr<T>;

        typedef std::pair<double, double> pair_dd;
        typedef std::pair<int, int> pair_ii;
        typedef std::pair<int, double> pair_id;
        typedef std::pair<double, int> pair_di;
    }
}

#endif
