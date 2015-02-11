#ifndef ARRAY1_INCLUDED_1279837123
#define ARRAY1_INCLUDED_1279837123

#include <cstddef>
#include <cassert>
#include <utility>
#include <vector>
#include <stdexcept>
#include "smart_index.h"
#include "index_iterator.h"
#include "traits.h"
#include "macros.h"
#include "proxy_iota.h"

namespace sx {

    template<typename T>
    class darray1;

    template<typename T, bool Mutable = false>
    class array1
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

        typedef array1<T, Mutable> this_type;

        array1() : data_(nullptr), size_(0), stride_(0) {
        }

        array1(pointer data, ssize_t size, ssize_t stride = 1) : data_(data), size_(size), stride_(stride) {
        }

        array1(pointer data, pointer end, ssize_t stride = 1)
                : data_(data), size_((end - data) / stride), stride_(stride) {
        }

        //intentionally not explicit
        array1(typename std::conditional<Mutable, std::vector<value_type>, const std::vector<value_type>>::type &v)
                : array1(v.data(), v.size()) {
        }

        //intentionally not explicit
        array1(typename std::conditional<Mutable, darray1<value_type>, const darray1<value_type>>::type &v);

        //intentionally not explicit
        //compile-time error if Mutable
        array1(const std::basic_string<value_type> &bs) : array1(bs.c_str(), (ssize_t) bs.size()) {
        }

        array1(const this_type &x) : data_(x.data_), size_(x.size_), stride_(x.stride_) {
        }

        array1(const this_type &&x) : data_(x.data_), size_(x.size_), stride_(x.stride_) {
        }

        this_type &operator=(const this_type &) = delete;

        this_type &operator=(this_type &&x) {
            data_ = x.data_;
            size_ = x.size_;
            stride_ = x.stride_;
            return *this;
        }

        //op[] is always const, just like char*const p is const
        reference operator[](ssize_t idx) const {
            assert(0 <= idx && idx < size_);
            return data_[idx * stride_];
        }

        reference operator[](smart_index sidx) const {
            return operator[](sidx.effective_idx_unchecked(size_));
        }

        template<typename S, typename std::enable_if<std::is_integral<S>::value>::type * = nullptr>
        std::vector<T> operator[](array1<S> idcs) const {
            std::vector<T> result;
            result.reserve(idcs.size());
            for (auto &x : idcs) result.push_back((*this)[x]);
            return result;
        }

        template<typename S, typename std::enable_if<std::is_integral<S>::value>::type * = nullptr>
        std::vector<T> operator[](const std::vector<S> &idcs) const {
            return operator[](array1<S>(idcs));
        }

        pointer const data() const {
            return data_;
        }

        ssize_t size() const {
            return size_;
        }

        ssize_t stride() const {
            return stride_;
        }

        this_type slice(smart_index lower, smart_index upper) const {
            ssize_t l = lower.effective_idx_unchecked(size_);
            ssize_t u = upper.effective_idx_unchecked(size_);
            assert(0 <= l && l < size_ && 0 <= u && u <= size_ && l <= u);
            if (l == u)
                return this_type();
            return this_type(data_ + l * stride_, u - l, stride_);
        }

        this_type slicen(smart_index lower, ssize_t n) const {
            return slice(lower, lower + n);
        }

        operator array1<T, false>() const {
            return array1<T, false>(data(), size(), stride());
        }

    private:
        pointer data_;
        ssize_t size_;
        ssize_t stride_;
    };

    template<typename T> using marray1 = array1<T, true>;

    template<typename T>
    class darray1
            : public container_traits_tags::indexable {
    public:
        typedef std::vector<T> container_type;
        typedef typename container_type::value_type value_type;
        typedef typename container_type::reference reference;
        typedef typename container_type::const_reference const_reference;
        typedef typename container_type::pointer pointer;
        typedef typename container_type::const_pointer const_pointer;
        typedef typename container_type::iterator iterator;
        typedef typename container_type::const_iterator const_iterator;
        typedef ssize_t size_type;

        typedef darray1<T> this_type;

        darray1() {
        }

        darray1(const this_type& x):v_(x.v_) {
        }
        darray1(this_type&& x) :v_(std::move(x.v_)) {
        }
        this_type& operator=(const this_type& x){
            v_ = x.v_;
            return *this;
        }
        this_type& operator=(this_type&& x){
            v_ = std::move(x.v_);
            return *this;
        }

        darray1(const_pointer p, ssize_t size) : v_(p, p + size) {
        }

        template<typename InputIt>
        darray1(InputIt first, InputIt last) : v_(first, last) {
        }

        template<typename E, typename std::enable_if<container_traits<E>::indexable>::type * = nullptr>
        explicit darray1(const E &e) {
            v_.reserve(e.size());
            for (auto i : iota(e.size())) v_.push_back(e[i]);
        }

        explicit darray1(ssize_t count) : v_(count) {
        }

        darray1(ssize_t count, const T &value) : v_(count, value) {
        }

        template<typename S>
        darray1(std::initializer_list<S> il) : darray1(il.begin(), il.end()) {
        }

        this_type &operator=(const array1<T> &y) {
            v_.assign(begin(y), end(y));
            return *this;
        }

        void clear() {
            v_.clear();
        }

        bool empty() const {
            return v_.empty();
        }

        reference operator[](ssize_t idx) {
            return v_[idx];
        }

        reference operator[](smart_index sidx) {
            return operator[](sidx.effective_idx_unchecked(size()));
        }

        const_reference operator[](ssize_t idx) const {
            return v_[idx];
        }

        const_reference operator[](smart_index sidx) const {
            return operator[](sidx.effective_idx_unchecked(size()));
        }

        pointer data() {
            return v_.data();
        }

        const_pointer data() const {
            return v_.data();
        }

        ssize_t size() const {
            return v_.size();
        }

        ssize_t stride() const {
            return 1;
        }

        marray1<T> slice(smart_index lower, smart_index upper) {
            return marray1<T>(*this).slice(lower, upper);
        }

        array1<T> slice(smart_index lower, smart_index upper) const {
            return array1<T>(*this).slice(lower, upper);
        }

        marray1<T> slicen(smart_index lower, ssize_t n) {
            return slice(lower, lower + n);
        }

        array1<T> slicen(smart_index lower, ssize_t n) const {
            return slicen(lower, lower + n);
        }

        iterator push_back(const T &x) {
            v_.push_back(x);
            return v_.end() - 1;
        }

        iterator push_back(T &&x) {
            v_.push_back(std::move(x));
            return v_.end() - 1;
        }

        template<typename...Args>
        iterator emplace_back(Args &&... args) {
            v_.emplace_back(std::forward<Args...>(args...));
            return v_.end() - 1;
        }

        iterator emplace_back() {
            v_.emplace_back();
            return v_.end() - 1;
        }

        void reserve(ssize_t x) {
            v_.reserve(x);
        }

        template<typename InputIt>
        void push_back(InputIt first, InputIt last) {
            v_.insert(v_.end(), first, last);
        }

        template<typename S>
        this_type operator/=(const S &x) {
            for (auto &y:v_) y /= x;
            return *this;
        }

        this_type &operator+=(array1<T> x) {
            if (size() != x.size())
                throw std::runtime_error("array1 operator+= different sizes");
            for (auto i : iota(x.size()))
                (*this)[i] += x[i];
            return *this;
        }

    private:
        container_type v_;
    };

    template<typename T, bool Mutable>
    array1<T, Mutable>::array1(typename std::conditional<Mutable, darray1<value_type>, const darray1<value_type>>::type &v)
            : array1(v.data(), v.size()) {
    }

    template<typename T>
    const_index_iterator<const darray1<T>>

    begin(const darray1<T> &that) {
        return const_index_iterator<const darray1<T>>(&that, 0);
    }

    template<typename T>
    const_index_iterator<const darray1<T>> end(const darray1<T> &that) {
        return const_index_iterator<const darray1<T>>(&that, that.size());
    }

    template<typename T>
    mutable_index_iterator<darray1<T>> begin(darray1<T> &that) {
        return mutable_index_iterator<darray1<T>>(&that, 0);
    }

    template<typename T>
    mutable_index_iterator<darray1<T>> end(darray1<T> &that) {
        return mutable_index_iterator<darray1<T>>(&that, that.size());
    }

#if 0
    template<typename T>
    class range_iterator {
    public:
        template<typename It>
        range_iterator(It it) :pimpl(new It(it)) {
            op_asterisk_fun = op_asterisk_fun_template < It > ;
        }
        reference operator*() const {
            return op_asterisk_fun(pimpl);
        }
        ~range_iterator(){
            delete_fun(pimpl);
        }
    private:
        void *pimpl;
        typedef reference(*op_asterisk_fun_t)(void*);
        void op_asterisk_fun_t op_asterisk_fun;

        template<typename It>
        static reference op_asterisk_fun_template(void* pimpl){
            return reinterpret_cast<It*>(pimpl)->operator*();
        }

        template<typename It>
        static void delete_fun(void*pimpl){
            delete reinterpret_cast<It*>(pimpl);
        }
    };

    template<typename T>
    class range {
    public:
        template<typename It>
        range(It first, It last):first(first),last(last){}

        range_iterator<T> begin() const {
            return first;
        }
        range_iterator<T> end() const {
            return last;
        }
    private:
        range_iterator<T> first, last;
    };
#endif
}

#endif
