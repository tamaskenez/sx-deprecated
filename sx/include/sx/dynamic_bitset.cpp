#include "sx/dynamic_bitset.h"

namespace sx {

    const dynamic_bitset::block_width_type
        dynamic_bitset::bits_per_block;


    const dynamic_bitset::size_type
        dynamic_bitset::npos;


    const dynamic_bitset::block_width_type
        dynamic_bitset::ulong_width;

    //=============================================================================
    // dynamic_bitset implementation


    //-----------------------------------------------------------------------------
    // constructors, etc.


    dynamic_bitset::dynamic_bitset()
        : m_num_bits(0)
    {

    }


    dynamic_bitset::
        dynamic_bitset(size_type num_bits, unsigned long value)
        : m_num_bits(0)
    {
        init_from_unsigned_long(num_bits, value);
    }

    // copy constructor

    inline dynamic_bitset::
        dynamic_bitset(const dynamic_bitset& b)
        : m_bits(b.m_bits), m_num_bits(b.m_num_bits)
    {

    }

    dynamic_bitset::
        dynamic_bitset(std::initializer_list<bool> il)
        : m_bits(calc_num_blocks(il.size()), 0), m_num_bits(il.size())
    {
        size_type counter = 0;
        for (auto b : il)
        {
            if (b) set(counter);
            ++counter;
        }
    }

    inline dynamic_bitset::
        ~dynamic_bitset()
    {
        assert(m_check_invariants());
    }


    void dynamic_bitset::
        swap(dynamic_bitset& b) // no throw
    {
        std::swap(m_bits, b.m_bits);
        std::swap(m_num_bits, b.m_num_bits);
    }


    dynamic_bitset& dynamic_bitset::
        operator=(const dynamic_bitset& b)
    {
        m_bits = b.m_bits;
        m_num_bits = b.m_num_bits;
        return *this;
    }


    inline dynamic_bitset::
        dynamic_bitset(dynamic_bitset&& b) noexcept
        : m_bits(std::move(b.m_bits)), m_num_bits(std::move(b.m_num_bits))
    {
        // Required so that assert(m_check_invariants()); works.
        assert((b.m_bits = buffer_type()).empty());
        b.m_num_bits = 0;
    }


    inline dynamic_bitset& dynamic_bitset::
        operator=(dynamic_bitset&& b) noexcept
    {
        if (std::addressof(b) == this) { return *this; }

        m_bits = std::move(b.m_bits);
        m_num_bits = std::move(b.m_num_bits);
        // Required so that assert(m_check_invariants()); works.
        assert((b.m_bits = buffer_type()).empty());
        b.m_num_bits = 0;
        return *this;
    }




        //-----------------------------------------------------------------------------
        // size changing operations


        void dynamic_bitset::
        resize(size_type num_bits, bool value) // strong guarantee
    {

        const size_type old_num_blocks = num_blocks();
        const size_type required_blocks = calc_num_blocks(num_bits);

        const block_type v = value ? ~Block(0) : Block(0);

        if (required_blocks != old_num_blocks) {
            m_bits.resize(required_blocks, v); // s.g. (copy)
        }


        // At this point:
        //
        //  - if the buffer was shrunk, we have nothing more to do,
        //    except a call to m_zero_unused_bits()
        //
        //  - if it was enlarged, all the (used) bits in the new blocks have
        //    the correct value, but we have not yet touched those bits, if
        //    any, that were 'unused bits' before enlarging: if value == true,
        //    they must be set.

        if (value && (num_bits > m_num_bits)) {

            const block_width_type extra_bits = count_extra_bits();
            if (extra_bits) {
                assert(old_num_blocks >= 1 && old_num_blocks <= m_bits.size());

                // Set them.
                m_bits[old_num_blocks - 1] |= (v << extra_bits);
            }

        }

        m_num_bits = num_bits;
        m_zero_unused_bits();

    }


    void dynamic_bitset::
        clear() // no throw
    {
        m_bits.clear();
        m_num_bits = 0;
    }



    void dynamic_bitset::
        push_back(bool bit)
    {
        const size_type sz = size();
        resize(sz + 1);
        set(sz, bit);
    }


    void dynamic_bitset::
        append(Block value) // strong guarantee
    {
        const block_width_type r = count_extra_bits();

        if (r == 0) {
            // the buffer is empty, or all blocks are filled
            m_bits.push_back(value);
        }
        else {
            m_bits.push_back(value >> (bits_per_block - r));
            m_bits[m_bits.size() - 2] |= (value << r); // m_bits.size() >= 2
        }

        m_num_bits += bits_per_block;
        assert(m_check_invariants());

    }


    //-----------------------------------------------------------------------------
    // bitset operations

    dynamic_bitset&
        dynamic_bitset::operator&=(const dynamic_bitset& rhs)
    {
        assert(size() == rhs.size());
        for (size_type i = 0; i < num_blocks(); ++i)
            m_bits[i] &= rhs.m_bits[i];
        return *this;
    }


    dynamic_bitset&
        dynamic_bitset::operator|=(const dynamic_bitset& rhs)
    {
        assert(size() == rhs.size());
        for (size_type i = 0; i < num_blocks(); ++i)
            m_bits[i] |= rhs.m_bits[i];
        //m_zero_unused_bits();
        return *this;
    }


    dynamic_bitset&
        dynamic_bitset::operator^=(const dynamic_bitset& rhs)
    {
        assert(size() == rhs.size());
        for (size_type i = 0; i < this->num_blocks(); ++i)
            m_bits[i] ^= rhs.m_bits[i];
        //m_zero_unused_bits();
        return *this;
    }


    dynamic_bitset&
        dynamic_bitset::operator-=(const dynamic_bitset& rhs)
    {
        assert(size() == rhs.size());
        for (size_type i = 0; i < num_blocks(); ++i)
            m_bits[i] &= ~rhs.m_bits[i];
        //m_zero_unused_bits();
        return *this;
    }

    //
    // NOTE:
    //  Note that the 'if (r != 0)' is crucial to avoid undefined
    //  behavior when the left hand operand of >> isn't promoted to a
    //  wider type (because rs would be too large).
    //

    dynamic_bitset&
        dynamic_bitset::operator<<=(size_type n)
    {
        if (n >= m_num_bits)
            return reset();
        //else
        if (n > 0) {

            size_type    const last = num_blocks() - 1;  // num_blocks() is >= 1
            size_type    const div = n / bits_per_block; // div is <= last
            block_width_type const r = bit_index(n);
            block_type * const b = &m_bits[0];

            if (r != 0) {

                block_width_type const rs = bits_per_block - r;

                for (size_type i = last - div; i > 0; --i) {
                    b[i + div] = (b[i] << r) | (b[i - 1] >> rs);
                }
                b[div] = b[0] << r;

            }
            else {
                for (size_type i = last - div; i > 0; --i) {
                    b[i + div] = b[i];
                }
                b[div] = b[0];
            }

            // zero out div blocks at the less significant end
            std::fill_n(m_bits.begin(), div, static_cast<block_type>(0));

            // zero out any 1 bit that flowed into the unused part
            m_zero_unused_bits(); // thanks to Lester Gong

        }

        return *this;


    }


    //
    // NOTE:
    //  see the comments to operator <<=
    //

    dynamic_bitset & dynamic_bitset::operator>>=(size_type n) {
        if (n >= m_num_bits) {
            return reset();
        }
        //else
        if (n > 0) {

            size_type  const last = num_blocks() - 1; // num_blocks() is >= 1
            size_type  const div = n / bits_per_block;   // div is <= last
            block_width_type const r = bit_index(n);
            block_type * const b = &m_bits[0];


            if (r != 0) {

                block_width_type const ls = bits_per_block - r;

                for (size_type i = div; i < last; ++i) {
                    b[i - div] = (b[i] >> r) | (b[i + 1] << ls);
                }
                // r bits go to zero
                b[last - div] = b[last] >> r;
            }

            else {
                for (size_type i = div; i <= last; ++i) {
                    b[i - div] = b[i];
                }
                // note the '<=': the last iteration 'absorbs'
                // b[last-div] = b[last] >> 0;
            }



            // div blocks are zero filled at the most significant end
            std::fill_n(m_bits.begin() + (num_blocks() - div), div, static_cast<block_type>(0));
        }

        return *this;
    }



    dynamic_bitset
        dynamic_bitset::operator<<(size_type n) const
    {
        dynamic_bitset r(*this);
        return r <<= n;
    }


    dynamic_bitset
        dynamic_bitset::operator>>(size_type n) const
    {
        dynamic_bitset r(*this);
        return r >>= n;
    }


    //-----------------------------------------------------------------------------
    // basic bit operations


    dynamic_bitset&
        dynamic_bitset::set(size_type pos, bool val)
    {
        assert(pos < m_num_bits);

        if (val)
            m_bits[block_index(pos)] |= bit_mask(pos);
        else
            reset(pos);

        return *this;
    }


    dynamic_bitset&
        dynamic_bitset::set()
    {
        std::fill(m_bits.begin(), m_bits.end(), ~Block(0));
        m_zero_unused_bits();
        return *this;
    }


    dynamic_bitset&
        dynamic_bitset::reset(size_type pos)
    {
        assert(pos < m_num_bits);
        m_bits[block_index(pos)] &= ~bit_mask(pos);
        return *this;
    }


    dynamic_bitset&
        dynamic_bitset::reset()
    {
        std::fill(m_bits.begin(), m_bits.end(), Block(0));
        return *this;
    }


    dynamic_bitset&
        dynamic_bitset::flip(size_type pos)
    {
        assert(pos < m_num_bits);
        m_bits[block_index(pos)] ^= bit_mask(pos);
        return *this;
    }


    dynamic_bitset&
        dynamic_bitset::flip()
    {
        for (size_type i = 0; i < num_blocks(); ++i)
            m_bits[i] = ~m_bits[i];
        m_zero_unused_bits();
        return *this;
    }


    bool dynamic_bitset::m_unchecked_test(size_type pos) const
    {
        return (m_bits[block_index(pos)] & bit_mask(pos)) != 0;
    }


    bool dynamic_bitset::test(size_type pos) const
    {
        assert(pos < m_num_bits);
        return m_unchecked_test(pos);
    }


    bool dynamic_bitset::test_set(size_type pos, bool val)
    {
        bool const b = test(pos);
        if (b != val) {
            set(pos, val);
        }
        return b;
    }


    bool dynamic_bitset::all() const
    {
        if (empty()) {
            return true;
        }

        const block_width_type extra_bits = count_extra_bits();
        block_type const all_ones = ~static_cast<Block>(0);

        if (extra_bits == 0) {
            for (size_type i = 0, e = num_blocks(); i < e; ++i) {
                if (m_bits[i] != all_ones) {
                    return false;
                }
            }
        }
        else {
            for (size_type i = 0, e = num_blocks() - 1; i < e; ++i) {
                if (m_bits[i] != all_ones) {
                    return false;
                }
            }
            block_type const mask = ~(~static_cast<Block>(0) << extra_bits);
            if (m_highest_block() != mask) {
                return false;
            }
        }
        return true;
    }


    bool dynamic_bitset::any() const
    {
        for (size_type i = 0; i < num_blocks(); ++i)
            if (m_bits[i])
                return true;
        return false;
    }


    inline bool dynamic_bitset::none() const
    {
        return !any();
    }


    dynamic_bitset
        dynamic_bitset::operator~() const
    {
        dynamic_bitset b(*this);
        b.flip();
        return b;
    }


    dynamic_bitset::size_type
        dynamic_bitset::count() const noexcept
    {
        using detail::dynamic_bitset_impl::table_width;
        using detail::dynamic_bitset_impl::access_by_bytes;
        using detail::dynamic_bitset_impl::access_by_blocks;
        using detail::dynamic_bitset_impl::value_to_type;

        // NOTE: Explicitly qualifying "bits_per_block" to workaround
        //       regressions of gcc 3.4.x
        enum {
            no_padding =
            dynamic_bitset::bits_per_block
            == CHAR_BIT * sizeof(Block)
        };

        enum { enough_table_width = table_width >= CHAR_BIT };

        enum {
            mode = (no_padding && enough_table_width)
            ? access_by_bytes
            : access_by_blocks
        };

        return do_count(m_bits.begin(), num_blocks(), Block(0),
            static_cast<value_to_type<(bool)mode> *>(0));
    }


        //-----------------------------------------------------------------------------
        // conversions

        inline dynamic_bitset::size_type
        dynamic_bitset::size() const noexcept
    {
        return m_num_bits;
    }


        inline dynamic_bitset::size_type
        dynamic_bitset::num_blocks() const noexcept
    {
        return m_bits.size();
    }

        inline bool dynamic_bitset::empty() const noexcept
    {
        return size() == 0;
    }


        bool dynamic_bitset::
        is_subset_of(const dynamic_bitset& a) const
    {
        assert(size() == a.size());
        for (size_type i = 0; i < num_blocks(); ++i)
            if (m_bits[i] & ~a.m_bits[i])
                return false;
        return true;
    }


    bool dynamic_bitset::
        is_proper_subset_of(const dynamic_bitset& a) const
    {
        assert(size() == a.size());
        assert(num_blocks() == a.num_blocks());

        bool proper = false;
        for (size_type i = 0; i < num_blocks(); ++i) {
            const Block & bt = m_bits[i];
            const Block & ba = a.m_bits[i];

            if (bt & ~ba)
                return false; // not a subset at all
            if (ba & ~bt)
                proper = true;
        }
        return proper;
    }


    bool dynamic_bitset::intersects(const dynamic_bitset & b) const
    {
        size_type common_blocks = num_blocks() < b.num_blocks()
            ? num_blocks() : b.num_blocks();

        for (size_type i = 0; i < common_blocks; ++i) {
            if (m_bits[i] & b.m_bits[i])
                return true;
        }
        return false;
    }

    // --------------------------------
    // lookup


    // look for the first bit "on", starting
    // from the block with index first_block
    //

    dynamic_bitset::size_type
        dynamic_bitset::m_do_find_from(size_type first_block) const
    {
        size_type i = first_block;

        // skip null blocks
        while (i < num_blocks() && m_bits[i] == 0)
            ++i;

        if (i >= num_blocks())
            return npos; // not found

        return i * bits_per_block + static_cast<size_type>(sx::lowest_bit(m_bits[i]));

    }



    dynamic_bitset::size_type
        dynamic_bitset::find_first() const
    {
        return m_do_find_from(0);
    }



    dynamic_bitset::size_type
        dynamic_bitset::find_next(size_type pos) const
    {

        const size_type sz = size();
        if (pos >= (sz - 1) || sz == 0)
            return npos;

        ++pos;

        const size_type blk = block_index(pos);
        const block_width_type ind = bit_index(pos);

        // shift bits upto one immediately after current
        const Block fore = m_bits[blk] >> ind;

        return fore ?
            pos + static_cast<size_type>(lowest_bit(fore))
            :
            m_do_find_from(blk + 1);

    }



    //-----------------------------------------------------------------------------
    // comparison


    bool operator==(const dynamic_bitset& a,
        const dynamic_bitset& b)
    {
        return (a.m_num_bits == b.m_num_bits)
            && (a.m_bits == b.m_bits);
    }


    inline bool operator!=(const dynamic_bitset& a,
        const dynamic_bitset& b)
    {
        return !(a == b);
    }


    bool operator<(const dynamic_bitset& a,
        const dynamic_bitset& b)
    {
        assert(a.size() == b.size());
        typedef dynamic_bitset::size_type size_type;

        //if (a.size() == 0)
        //  return false;

        // Since we are storing the most significant bit
        // at pos == size() - 1, we need to do the comparisons in reverse.
        //
        for (size_type ii = a.num_blocks(); ii > 0; --ii) {
            size_type i = ii - 1;
            if (a.m_bits[i] < b.m_bits[i])
                return true;
            else if (a.m_bits[i] > b.m_bits[i])
                return false;
        }
        return false;
    }


    inline bool operator<=(const dynamic_bitset& a,
        const dynamic_bitset& b)
    {
        return !(a > b);
    }


    inline bool operator>(const dynamic_bitset& a,
        const dynamic_bitset& b)
    {
        return b < a;
    }


    inline bool operator>=(const dynamic_bitset& a,
        const dynamic_bitset& b)
    {
        return !(a < b);
    }


    //-----------------------------------------------------------------------------
    // bitset operations


    dynamic_bitset
        operator&(const dynamic_bitset& x,
        const dynamic_bitset& y)
    {
        dynamic_bitset b(x);
        return b &= y;
    }


    dynamic_bitset
        operator|(const dynamic_bitset& x,
        const dynamic_bitset& y)
    {
        dynamic_bitset b(x);
        return b |= y;
    }


    dynamic_bitset
        operator^(const dynamic_bitset& x,
        const dynamic_bitset& y)
    {
        dynamic_bitset b(x);
        return b ^= y;
    }


    dynamic_bitset
        operator-(const dynamic_bitset& x,
        const dynamic_bitset& y)
    {
        dynamic_bitset b(x);
        return b -= y;
    }

    //-----------------------------------------------------------------------------
    // namespace scope swap


    void
        swap(dynamic_bitset& left,
        dynamic_bitset& right) // no throw
    {
        left.swap(right);
    }


    //-----------------------------------------------------------------------------
    // private (on conforming compilers) member functions



    inline dynamic_bitset::size_type
        dynamic_bitset::calc_num_blocks(size_type num_bits)
    {
        return num_bits / bits_per_block
            + static_cast<size_type>(num_bits % bits_per_block != 0);
    }

    // gives a reference to the highest block
    //

    inline dynamic_bitset::Block& dynamic_bitset::m_highest_block()
    {
        return const_cast<Block &>
            (static_cast<const dynamic_bitset *>(this)->m_highest_block());
    }

    // gives a const-reference to the highest block
    //

    inline const dynamic_bitset::Block& dynamic_bitset::m_highest_block() const
    {
        assert(size() > 0 && num_blocks() > 0);
        return m_bits.back();
    }


    // If size() is not a multiple of bits_per_block
    // then not all the bits in the last block are used.
    // This function resets the unused bits (convenient
    // for the implementation of many member functions)
    //

    inline void dynamic_bitset::m_zero_unused_bits()
    {
        assert(num_blocks() == calc_num_blocks(m_num_bits));

        // if != 0 this is the number of bits used in the last block
        const block_width_type extra_bits = count_extra_bits();

        if (extra_bits != 0)
            m_highest_block() &= ~(~static_cast<Block>(0) << extra_bits);

    }

    // check class invariants

    bool dynamic_bitset::m_check_invariants() const
    {
        const block_width_type extra_bits = count_extra_bits();
        if (extra_bits > 0) {
            block_type const mask = (~static_cast<Block>(0) << extra_bits);
            if ((m_highest_block() & mask) != 0)
                return false;
        }
        if (m_bits.size() > m_bits.capacity() || num_blocks() != calc_num_blocks(size()))
            return false;

        return true;

    }

}