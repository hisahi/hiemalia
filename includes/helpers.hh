/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// helpers.hh: various helpers

#ifndef M_HELPERS_HH
#define M_HELPERS_HH

#include <algorithm>

namespace hiemalia {
template <typename T1, typename T2>
inline bool erase_remove(std::vector<T1> v, T2 x) {
    auto it = std::remove(v.begin(), v.end(), x);
    bool found = it != v.end();
    v.erase(it, v.end());
    return found;
}
}  // namespace hiemalia

#endif  // M_HELPERS_HH
