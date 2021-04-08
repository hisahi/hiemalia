/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// sort.hh: sort helpers

#ifndef M_SORT_HH
#define M_SORT_HH

#include <algorithm>
#include <numeric>
#include <vector>

namespace hiemalia {

template <typename RandomIt, typename Compare>
std::vector<size_t> stableIndexSort(RandomIt first, RandomIt last,
                                    Compare cmp) {
    size_t i = 1, j, n = last - first;
    std::vector<size_t> idx(n);
    std::iota(idx.begin(), idx.end(), 0);
    while (i < n) {
        j = i++;
        while (j > 0 && cmp(first[j], first[j - 1])) {
            std::swap(first[j], first[j - 1]);
            std::swap(idx[j], idx[j - 1]);
            --j;
        }
    }
    return idx;
}
}  // namespace hiemalia

#endif  // M_SORT_HH
