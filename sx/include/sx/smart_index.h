#ifndef SMART_INDEX_INCLUDED_184623
#define SMART_INDEX_INCLUDED_184623

#include "types.h"

namespace sx {
    struct smart_index {
        smart_index(ssize_t idx, bool from_end = false, bool can_clamp = false)
                : raw_idx(idx), from_end(from_end), can_clamp(can_clamp) {
        }

        ssize_t effective_idx_unchecked(ssize_t size) const {
            ssize_t r = raw_idx;
            if (from_end)
                r += size;
            if (can_clamp) {
                if (r < 0)
                    r = 0;
                else if (r > size)
                    r = size;
            }
            return r;
        }

        ssize_t raw_idx;
        bool from_end;
        bool can_clamp;
    };

    smart_index from_end(ssize_t idx) {
        return smart_index(idx, true);
    }

    smart_index from_end_clamped(ssize_t idx) {
        return smart_index(idx, true, true);
    }

    smart_index from_start_clamped(ssize_t idx) {
        return smart_index(idx, false, true);
    }

    smart_index operator+(smart_index x, ssize_t n) {
        return smart_index(x.raw_idx + n, x.from_end, x.can_clamp);
    }

}

#endif
