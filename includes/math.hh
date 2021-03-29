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
namespace numbers {
inline const double PI = 3.14159265358979323846264338327950288419716939937510;
inline const double TAU = 2 * PI;
}  // namespace numbers

template <typename T>
constexpr T radians(T degrees) {
    return degrees * (numbers::PI / 180);
}

template <typename T>
constexpr T degrees(T radians) {
    return radians * (180 / numbers::PI);
}

template <typename T>
constexpr T lerp(T start, T x, T end) {
    return x * end + (1 - x) * start;
}

template <typename T>
constexpr T unlerp(T start, T x, T end) {
    return (x - start) / (end - start);
}

template <typename Ti, typename Tf>
Ti floatToWholeFrac(Tf& x) {
    Ti i = static_cast<Ti>(std::floor(x));
    x -= i;
    return i;
}

template <typename T>
constexpr T wrapAngle(T x) {
    return std::fmod(x, numbers::TAU);
}
};  // namespace hiemalia

#endif  // M_MATH_HH
