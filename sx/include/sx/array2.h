#ifndef ARRAY2_INCLUDED_1279837123
#define ARRAY2_INCLUDED_1279837123

#include <array>
#include <cstddef>
#include <cassert>
#include <utility>

#include "smart_index.h"
#include "index_iterator.h"
#include "traits.h"
#include "array1.h"

namespace sx {

    template<typename T>
    class darray2;

    template<typename T, bool Mutable = false>
    class array2
            : public container_traits_tags::indexable,
              public std::conditional<Mutable, container_traits_tags::use_mutable_index_iterator, container_traits_tags::use_const_index_iterator>::type {
    public:
        typedef typename std::remove_const<T>::type value_type;
        typedef typename std::add_const<T>::type const_value_type;
        typedef typename std::conditional<Mutable, value_type, const_value_type>::type &reference;
        typedef const_value_type &const_reference;
        typedef typename std::conditional<Mutable, value_type, const_value_type>::type *pointer;
        typedef const_value_type *const_pointer;
        typedef ssize_t size_type;

        typedef array2<T, Mutable> this_type;

        array2() : data_(nullptr) {
            sizes_.fill(0);
            strides_.fill(0);
        }

        array2(pointer data0, ssize_t nr0, ssize_t nc0)
                : data_(data0)
#ifndef _MSVC
        {
            sizes_[0] = nr0;
            sizes_[1] = nc0;
            strides_[0] = nc0;
            strides_[1] = 1;
#else
            , sizes_{ nr, nc }, strides_{ row_stride, col_stride } {
#endif
        };

        array2(pointer data0, ssize_t nr0, ssize_t nc0, ssize_t row_stride0, ssize_t col_stride0 = 1)
                : data_(data0)
#ifndef _MSVC
        {
            sizes_[0] = nr0;
            sizes_[1] = nc0;
            strides_[0] = row_stride0;
            strides_[1] = col_stride0;
#else
            , sizes_{nr, nc}, strides_{row_stride, col_stride} {
#endif
        };

        //intentionally not explicit
        array2(typename std::conditional<Mutable, darray2<value_type>, const darray2<value_type>>::type &v);

        //op[] is always const, just like char*const p is const
        //linear index, row-major
        reference operator[](ssize_t idx) const {
            assert(0 <= idx && idx < size());
            return data_[idx / nc(), idx % nc()];
        }

        reference operator()(ssize_t row, ssize_t col) const {
            assert(0 <= row && row < nr() && 0 <= col && col < nc());
            return data_[row * strides_[0] + col * strides_[1]];
        }

        reference operator()(smart_index srow, smart_index scol) const {
            ssize_t row = srow.effective_idx_unchecked(nr());
            ssize_t col = scol.effective_idx_unchecked(nc());
            return (*this)(row, col);
        }

        reference at(ssize_t row, ssize_t col) const {
            return (*this)(row, col);
        }

        reference at(smart_index row, smart_index col) const {
            return (*this)(row, col);
        }

        pointer const data() const {
            return data_;
        }

        ssize_t nr() const {
            return sizes_[0];
        }

        ssize_t nc() const {
            return sizes_[1];
        }

        ssize_t size() const {
            return sizes_[0] * sizes_[1];
        }

        std::array<int, 2> strides() const {
            return strides_;
        }

        //submatrix of rows [r0, r1) and columns [c0, c1) (right-open intervals)
        this_type block(smart_index r0, smart_index r1, smart_index c0, smart_index c1) const {
            ssize_t R0 = r0.effective_idx_unchecked(sizes_[0]);
            ssize_t R1 = r1.effective_idx_unchecked(sizes_[0]);
            ssize_t C0 = c0.effective_idx_unchecked(sizes_[1]);
            ssize_t C1 = c1.effective_idx_unchecked(sizes_[1]);
            assert(0 <= R0 && R0 < sizes_[0] && 0 <= R1 && R1 <= sizes_[0] && R0 <= R1);
            assert(0 <= C0 && C0 < sizes_[0] && 0 <= C1 && C1 <= sizes_[0] && C0 <= C1);
            if (R0 == R1 || C0 == C1)
                return this_type();
            return this_type(&at(R0, C0), R1 - R0, C1 - C0, strides_[0], strides_[1]);
        }

        this_type blockn(smart_index r0, smart_index c0, ssize_t h, ssize_t w) const {
            return block(r0, r0 + h, c0, c0 + w);
        }

        //single or multiple rows/columns, intervals are right-open
        array1<T, Mutable> row(smart_index r0) const {
            return array1<T, Mutable>(&at(r0, 0), nc(), strides_[1]);
        }

        this_type rows(smart_index r0, smart_index r1) const {
            return block(r0, r1, 0, nc());
        }

        this_type rowsn(smart_index r0, ssize_t n) const {
            return block(r0, r0 + n, 0, nc());
        }

        array1<T, Mutable> col(smart_index c0) const {
            return array1<T, Mutable>(&at(0, c0), nr(), strides_[0]);
        }

        this_type cols(smart_index c0, smart_index c1) const {
            return block(0, nr(), c0, c1);
        }

        this_type colsn(smart_index c0, int n) const {
            return block(0, nr(), c0, c0 + n);
        }

    private:
        pointer data_;
        std::array<ssize_t, 2> sizes_;
        std::array<ssize_t, 2> strides_;
    };

    template<typename T> using marray2 = array2<T, true>;

    template<typename T>
    class darray2
            : public container_traits_tags::indexable {
    public:
        typedef std::vector<T> container_type;
        typedef typename container_type::value_type value_type;
        typedef typename container_type::reference reference;
        typedef typename container_type::const_reference const_reference;
        typedef typename container_type::pointer pointer;
        typedef typename container_type::const_pointer const_pointer;
        typedef darray2<T> this_type;
        typedef ssize_t size_type;

        darray2() : nr_(0), nc_(0) {
        }

        darray2(const this_type& x) :v_(x.v_),nr_(x.nr_),nc_(x.nc_) {
        }
        darray2(this_type&& x) :v_(std::move(x.v_)), nr_(x.nr_), nc_(x.nc_) {
            x.nr_ = x.nc_ = 0;
        }
        this_type& operator=(const this_type& x){
            v_ = x.v_;
            nr_ = x.nr_;
            nc_ = x.nc_;
            return *this;
        }
        this_type& operator=(this_type&& x){
            v_ = std::move(x.v_);
            nr_ = x.nr_;
            nc_ = x.nc_;
            x.nr_ = x.nc_ = 0;
            return *this;
        }

        template<bool Mutable>
        darray2(const array2<T, Mutable> &v) : v_(BEGINEND(v)), nr_(v.nr()), nc_(v.nc()) {
        }

        darray2(ssize_t nrows, ssize_t ncols) :
                v_(nrows * ncols), nr_(nrows), nc_(ncols) {
        }

        darray2(ssize_t nrows, ssize_t ncols, const value_type &x) :
                v_(nrows * ncols, x), nr_(nrows), nc_(ncols) {
        }

        void resize(ssize_t nrows, ssize_t ncols) {
            v_.resize(nrows * ncols);
            nr_ = nrows;
            nc_ = ncols;
        }

        void resize(ssize_t nrows, ssize_t ncols, const value_type &x) {
            v_.resize(nrows * ncols, x);
            nr_ = nrows;
            nc_ = ncols;
        }

        reference operator[](ssize_t idx) {
            return v_[idx];
        }

        const_reference operator[](ssize_t idx) const {
            return v_[idx];
        }

        pointer data() {
            return v_.data();
        }

        ssize_t nr() const {
            return nr_;
        }

        ssize_t nc() const {
            return nc_;
        }

        ssize_t size() const {
            return nr_ * nc_;
        }


        const_reference operator()(ssize_t row, ssize_t col) const {
            if (!(0 <= row && row < nr() && 0 <= col && col < nc())) {
                fprintf(stderr, "%d %d %d %d\n", (int) nr(), (int) nc(), (int) row, (int) col);
            }
            assert(0 <= row && row < nr() && 0 <= col && col < nc());
            return v_[row * nc_ + col];
        }

        const_reference operator()(smart_index srow, smart_index scol) const {
            ssize_t row = srow.effective_idx_unchecked(nr());
            ssize_t col = scol.effective_idx_unchecked(nc());
            return (*this)(row, col);
        }

        reference operator()(ssize_t row, ssize_t col) {
            if (!(0 <= row && row < nr() && 0 <= col && col < nc())) {
                fprintf(stderr, "%d %d %d %d\n", (int) nr(), (int) nc(), (int) row, (int) col);
            }
            assert(0 <= row && row < nr() && 0 <= col && col < nc());
            return v_[row * nc_ + col];
        }

        reference operator()(smart_index srow, smart_index scol) {
            ssize_t row = srow.effective_idx_unchecked(nr());
            ssize_t col = scol.effective_idx_unchecked(nc());
            return (*this)(row, col);
        }

        const_reference at(ssize_t row, ssize_t col) const {
            return (*this)(row, col);
        }

        const_reference at(smart_index row, smart_index col) const {
            return (*this)(row, col);
        }

        reference at(ssize_t row, ssize_t col) {
            return (*this)(row, col);
        }

        reference at(smart_index row, smart_index col) {
            return (*this)(row, col);
        }

        const_pointer data() const {
            return v_.data();
        }

        //operator array2<T, false>() const {
        //    return array2<T, false>(data(), nr(), nc());
        //}
        //
        //operator array2<T, false>() {
        //    return array2<T, false>(data(), nr(), nc());
        //}
        //
        //operator marray2<T>() {
        //    return marray2<T>(data(), nr(), nc());
        //}

        //submatrix of rows [r0, r1) and columns [c0, c1) (right-open intervals)
        array2<T> block(smart_index r0, smart_index r1, smart_index c0, smart_index c1) const {
            return array2<T>(*this).block(r0, r1, c0, c1);
        }

        marray2<T> block(smart_index r0, smart_index r1, smart_index c0, smart_index c1) {
            return marray2<T>(*this).block(r0, r1, c0, c1);
        }

        array2<T> blockn(smart_index r0, smart_index c0, ssize_t h, ssize_t w) const {
            return array2<T>(*this).block(r0, r0 + h, c0, c0 + w);
        }

        marray2<T> blockn(smart_index r0, smart_index c0, ssize_t h, ssize_t w) {
            return marray2<T>(*this).block(r0, r0 + h, c0, c0 + w);
        }

        //single or multiple rows/columns, intervals are right-open
        array1<T> row(smart_index r0) const {
            return array1<T>(&at(r0, 0), nc());
        }

        //single or multiple rows/columns, intervals are right-open
        marray1<T> row(smart_index r0) {
            return marray1<T>(&at(r0, 0), nc());
        }

        array2<T> rows(smart_index r0, smart_index r1) const {
            return block(r0, r1, 0, nc());
        }

        marray2<T> rows(smart_index r0, smart_index r1) {
            return block(r0, r1, 0, nc());
        }

        array2<T> rowsn(smart_index r0, ssize_t n) const {
            return block(r0, r0 + n, 0, nc());
        }

        marray2<T> rowsn(smart_index r0, ssize_t n) {
            return block(r0, r0 + n, 0, nc());
        }

        array1<T> col(smart_index c0) const {
            return array1<T>(&at(0, c0), nr(), nc());
        }

        marray1<T> col(smart_index c0) {
            return marray1<T>(&at(0, c0), nr(), nc());
        }

        array2<T> cols(smart_index c0, smart_index c1) const {
            return block(0, nr(), c0, c1);
        }

        marray2<T> cols(smart_index c0, smart_index c1) {
            return block(0, nr(), c0, c1);
        }

        array2<T> colsn(smart_index c0, int n) const {
            return block(0, nr(), c0, c0 + n);
        }

        marray2<T> colsn(smart_index c0, int n) {
            return block(0, nr(), c0, c0 + n);
        }

        template<typename S>
        this_type operator/=(const S &x) {
            for (auto &y:v_) y /= x;
            return *this;
        }

        void append_row(array1<T> v) {
            if (nr_ == 0) {
                if (v.size() == 0)
                    throw std::runtime_error("append_row: empty arg");
                nc_ = v.size();
            }
            else {
                if (v.size() != nc_)
                    throw std::runtime_error("append_row: invalid arg size");
            }
            v_.insert(v_.end(), BEGINEND(v));
            ++nr_;
        }

    private:
        container_type v_;
        ssize_t nr_, nc_;
    };

    template<typename T, bool Mutable>
    array2<T, Mutable>::array2(typename std::conditional<Mutable, darray2<value_type>, const darray2<value_type>>::type &v)
            : array2<T, Mutable>(v.data(), v.nr(), v.nc()) {
    }

    template<typename T>
    const_index_iterator<const darray2<T>> begin(const darray2<T> &that) {
        return const_index_iterator<const darray2<T>>(&that, 0);
    }

    template<typename T>
    const_index_iterator<const darray2<T>> end(const darray2<T> &that) {
        return const_index_iterator<const darray2<T>>(&that, that.size());
    }

    template<typename T>
    mutable_index_iterator<darray2<T>> begin(darray2<T> &that) {
        return mutable_index_iterator<darray2<T>>(&that, 0);
    }

    template<typename T>
    mutable_index_iterator<darray2<T>> end(darray2<T> &that) {
        return mutable_index_iterator<darray2<T>>(&that, that.size());
    }

}

#endif
