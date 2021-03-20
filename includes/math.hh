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
T radians(T degrees) {
    return degrees * (M_PI / 180.0);
}

template <typename T>
T degrees(T radians) {
    return radians * (180.0 / M_PI);
}
};  // namespace hiemalia

#endif  // M_MATH_HH
