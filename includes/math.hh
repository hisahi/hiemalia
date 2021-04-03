/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// math.hh: header file for math helpers

#ifndef M_MATH_HH
#define M_MATH_HH

#include <algorithm>
#include <cmath>

namespace hiemalia {
template <typename T>
constexpr T abs(T x) {
    return std::abs(x);
}

template <typename T>
constexpr T sqrt(T x) {
    return std::sqrt(x);
}

template <typename T>
constexpr T sin(T x) {
    return std::sin(x);
}

template <typename T>
constexpr T cos(T x) {
    return std::cos(x);
}

template <typename T>
constexpr T tan(T x) {
    return std::tan(x);
}

template <typename T>
constexpr T pow(T x, T y) {
    return std::pow(x, y);
}

template <typename T>
constexpr T acos(T x) {
    return std::acos(x);
}

namespace numbers {
template <typename T = double>
inline const T PI = static_cast<T>(
    3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679);
template <typename T = double>
inline const T TAU = 2 * PI<T>;
template <typename T = double>
inline const T SQRT2 = static_cast<T>(
    1.4142135623730950488016887242096980785696718753769480731766797379907324784621070388503875343276415727);
template <typename T = double>
inline const T ISQRT2 = 1 / SQRT2<T>;
}  // namespace numbers

template <typename T>
constexpr T radians(T degrees) {
    return degrees * (numbers::PI<T> / 180);
}

template <typename T>
constexpr T degrees(T radians) {
    return radians * (180 / numbers::PI<T>);
}

template <typename T>
constexpr T lerp(T start, T x, T end) {
    return start + x * (end - start);
}

template <typename T>
constexpr T sqerp(T start, T x, T end) {
    return start +
           (x >= 0.5 ? 1 - (2 * (1 - x) * (1 - x)) : 2 * x * x) * (end - start);
}

template <typename T>
constexpr T unlerp(T start, T x, T end) {
    return (x - start) / (end - start);
}

template <typename T>
constexpr T clamp(T min, T x, T max) {
    dynamic_assert(min <= max, "invalid clamp");
    return std::clamp(x, min, max);
}

template <typename T>
T frac(const T& x) {
    return x - std::floor(x);
}

template <typename Ti, typename Tf>
Ti floatToWholeFrac(Tf& x) {
    Ti i = static_cast<Ti>(std::floor(x));
    x -= i;
    return i;
}

template <typename T>
constexpr T wrapAngle(T x) {
    return std::fmod(x, numbers::TAU<T>);
}
};  // namespace hiemalia

#endif  // M_MATH_HH
