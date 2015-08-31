//taken from boost
// -----------------------------------------------------------
//
//   Copyright (c) 2001-2002 Chuck Allison and Jeremy Siek
//        Copyright (c) 2003-2006, 2008 Gennaro Prota
//             Copyright (c) 2014 Ahmed Charles
//
// Copyright (c) 2014 Glen Joseph Fernandes
// glenfe at live dot com
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// -----------------------------------------------------------

#ifndef STDAUX_DYNAMIC_BITSET_INCLUDED
#define STDAUX_DYNAMIC_BITSET_INCLUDED

#include <vector>
#include <cstdint>
#include <limits>
#include <cassert>
#include <utility>
#include <initializer_list>

#include "sx/dynamic_bitset/dynamic_bitset_impl.h"
#include "sx/integer/lowest_bit.h"

#if defined _NOEXCEPT && !defined noexcept
#define noexcept _NOEXCEPT
#endif

namespace sx {

class dynamic_bitset
{
    typedef uint64_t Block; //this was template par
    typedef std::vector<Block> buffer_type;

public:
    typedef Block block_type;
    typedef std::size_t size_type;
    typedef buffer_type::size_type block_width_type;

    static const block_width_type bits_per_block = std::numeric_limits<Block>::digits;
    static const size_type npos = static_cast<size_type>(-1);


public:

    // A proxy class to simulate lvalues of bit type.
    //
    class reference
    {
        friend class dynamic_bitset;


        // the one and only non-copy ctor
        reference(block_type & b, block_type pos)
            :m_block(b),
             m_mask( (assert(pos < bits_per_block),
                      block_type(1) << pos )
                   )
        { }

        void operator&(); // left undefined

    public:

        // copy constructor: compiler generated

        operator bool() const { return (m_block & m_mask) != 0; }
        bool operator~() const { return (m_block & m_mask) == 0; }

        reference& flip() { do_flip(); return *this; }

        reference& operator=(bool x)               { do_assign(x);   return *this; } // for b[i] = x
        reference& operator=(const reference& rhs) { do_assign(rhs); return *this; } // for b[i] = b[j]

        reference& operator|=(bool x) { if  (x) do_set();   return *this; }
        reference& operator&=(bool x) { if (!x) do_reset(); return *this; }
        reference& operator^=(bool x) { if  (x) do_flip();  return *this; }
        reference& operator-=(bool x) { if  (x) do_reset(); return *this; }

     private:
        block_type & m_block;
        const block_type m_mask;

        void do_set() { m_block |= m_mask; }
        void do_reset() { m_block &= ~m_mask; }
        void do_flip() { m_block ^= m_mask; }
        void do_assign(bool x) { x? do_set() : do_reset(); }
    };

    typedef bool const_reference;

    // constructors, etc.
    explicit
    dynamic_bitset();

    explicit
    dynamic_bitset(size_type num_bits, unsigned long value = 0);

    explicit
    dynamic_bitset(std::initializer_list<bool> il);

    // copy constructor
    dynamic_bitset(const dynamic_bitset& b);

    ~dynamic_bitset();

    void swap(dynamic_bitset& b);
    dynamic_bitset& operator=(const dynamic_bitset& b);

    dynamic_bitset(dynamic_bitset&& src) noexcept;
    dynamic_bitset& operator=(dynamic_bitset&& src) noexcept;

    // size changing operations
    void resize(size_type num_bits, bool value = false);
    void clear();
    void push_back(bool bit);
    void append(Block block);

    // bitset operations
    dynamic_bitset& operator&=(const dynamic_bitset& b);
    dynamic_bitset& operator|=(const dynamic_bitset& b);
    dynamic_bitset& operator^=(const dynamic_bitset& b);
    dynamic_bitset& operator-=(const dynamic_bitset& b);
    dynamic_bitset& operator<<=(size_type n);
    dynamic_bitset& operator>>=(size_type n);
    dynamic_bitset operator<<(size_type n) const;
    dynamic_bitset operator>>(size_type n) const;

    // basic bit operations
    dynamic_bitset& set(size_type n, bool val = true);
    dynamic_bitset& set();
    dynamic_bitset& reset(size_type n);
    dynamic_bitset& reset();
    dynamic_bitset& flip(size_type n);
    dynamic_bitset& flip();
    bool test(size_type n) const;
    bool test_set(size_type n, bool val = true);
    bool all() const;
    bool any() const;
    bool none() const;
    dynamic_bitset operator~() const;
    size_type count() const noexcept;

    // subscript
    reference operator[](size_type pos) {
        return reference(m_bits[block_index(pos)], bit_index(pos));
    }
    bool operator[](size_type pos) const { return test(pos); }

    unsigned long to_ulong() const;

    size_type size() const noexcept;
    size_type num_blocks() const noexcept;
    size_type max_size() const noexcept;
    bool empty() const noexcept;

    bool is_subset_of(const dynamic_bitset& a) const;
    bool is_proper_subset_of(const dynamic_bitset& a) const;
    bool intersects(const dynamic_bitset & a) const;

    // lookup
    size_type find_first() const;
    size_type find_next(size_type pos) const;


    // lexicographical comparison
    
    friend bool operator==(const dynamic_bitset& a,
                           const dynamic_bitset& b);

    
    friend bool operator<(const dynamic_bitset& a,
                          const dynamic_bitset& b);


    template <typename B, typename A, typename BlockOutputIterator>
    friend void to_block_range(const dynamic_bitset& b,
                               BlockOutputIterator result);

    template <typename BlockIterator, typename B, typename A>
    friend void from_block_range(BlockIterator first, BlockIterator last,
                                 dynamic_bitset& result);


    template <typename CharT, typename Traits, typename B, typename A>
    friend std::basic_istream<CharT, Traits>& operator>>(std::basic_istream<CharT, Traits>& is,
                                                         dynamic_bitset& b);

    template <typename B, typename A, typename stringT>
    friend void to_string_helper(const dynamic_bitset & b, stringT & s, bool dump_all);


    void init_from_unsigned_long(size_type num_bits,
        unsigned long value/*,
                           const Allocator& alloc*/)
    {

        assert(m_bits.size() == 0);

        m_bits.resize(calc_num_blocks(num_bits));
        m_num_bits = num_bits;

        typedef unsigned long num_type;
        typedef detail::dynamic_bitset_impl
            ::shifter<num_type, bits_per_block, ulong_width> shifter;

        //if (num_bits == 0)
        //    return;

        // zero out all bits at pos >= num_bits, if any;
        // note that: num_bits == 0 implies value == 0
        if (num_bits < static_cast<size_type>(ulong_width)) {
            const num_type mask = (num_type(1) << num_bits) - 1;
            value &= mask;
        }

        buffer_type::iterator it = m_bits.begin();
        for (; value; shifter::left_shift(value), ++it) {
            *it = static_cast<block_type>(value);
        }

    }


private:
    static const block_width_type ulong_width = std::numeric_limits<unsigned long>::digits;

    void m_zero_unused_bits();
    bool m_check_invariants() const;

    size_type m_do_find_from(size_type first_block) const;

    block_width_type count_extra_bits() const noexcept { return bit_index(size()); }
    static size_type block_index(size_type pos) noexcept{ return pos / bits_per_block; }
    static block_width_type bit_index(size_type pos) noexcept{ return static_cast<block_width_type>(pos % bits_per_block); }
    static Block bit_mask(size_type pos) noexcept{ return Block(1) << bit_index(pos); }

private:

    bool m_unchecked_test(size_type pos) const;
    static size_type calc_num_blocks(size_type num_bits);

    Block&        m_highest_block();
    const Block&  m_highest_block() const;

    buffer_type m_bits;
    size_type   m_num_bits;


    class bit_appender;
    friend class bit_appender;
    class bit_appender {
      // helper for stream >>
      // Supplies to the lack of an efficient append at the less
      // significant end: bits are actually appended "at left" but
      // rearranged in the destructor. From the perspective of
      // client code everything works *as if* dynamic_bitset<> had
      // an append_at_right() function (eventually throwing the same
      // exceptions as push_back) except that the function is in fact
      // called bit_appender::do_append().
      //
      dynamic_bitset & bs;
      size_type n;
      Block mask;
      Block * current;

      // not implemented
      bit_appender(const bit_appender &);
      bit_appender & operator=(const bit_appender &);

    public:
        bit_appender(dynamic_bitset & r) : bs(r), n(0), mask(0), current(0) {}
        ~bit_appender() {
            // reverse the order of blocks, shift
            // if needed, and then resize
            //
            std::reverse(bs.m_bits.begin(), bs.m_bits.end());
            const block_width_type offs = bit_index(n);
            if (offs)
                bs >>= (bits_per_block - offs);
            bs.resize(n); // doesn't enlarge, so can't throw
            assert(bs.m_check_invariants());
        }
        inline void do_append(bool value) {

            if (mask == 0) {
                bs.append(Block(0));
                current = &bs.m_highest_block();
                mask = Block(1) << (bits_per_block - 1);
            }

            if(value)
                *current |= mask;

            mask /= 2;
            ++n;
        }
        size_type get_count() const { return n; }
    };

};




// Global Functions:

// comparison

bool operator!=(const dynamic_bitset& a,
                const dynamic_bitset& b);


bool operator<=(const dynamic_bitset& a,
                const dynamic_bitset& b);


bool operator>(const dynamic_bitset& a,
               const dynamic_bitset& b);


bool operator>=(const dynamic_bitset& a,
                const dynamic_bitset& b);

// bitset operations

dynamic_bitset
operator&(const dynamic_bitset& b1,
          const dynamic_bitset& b2);


dynamic_bitset
operator|(const dynamic_bitset& b1,
          const dynamic_bitset& b2);


dynamic_bitset
operator^(const dynamic_bitset& b1,
          const dynamic_bitset& b2);


dynamic_bitset
operator-(const dynamic_bitset& b1,
          const dynamic_bitset& b2);

// namespace scope swap

void swap(dynamic_bitset& b1,
          dynamic_bitset& b2);




} // namespace boost


#undef BOOST_BITSET_CHAR

#endif // include guard

