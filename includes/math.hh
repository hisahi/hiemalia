/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// math.hh: header file for math helpers

#ifndef M_MATH_HH
#define M_MATH_HH

#include <cmath>

namespace hiemalia {
template <typename T>
constexpr T radians(T degrees) {
    return degrees * (M_PI / 180);
}

template <typename T>
constexpr T degrees(T radians) {
    return radians * (180 / M_PI);
}

template <typename T>
constexpr T lerp(T start, T x, T end) {
    return x * end + (1 - x) * start;
}
};  // namespace hiemalia

#endif  // M_MATH_HH
