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

#include "defs.hh"

namespace hiemalia {
namespace numbers {
template <typename T = coord_t>
inline const T PI = T{
    3.141592653589793238462643383279502884197169399375105820974944592307816406};
template <typename T = coord_t>
inline const T E = T{
    2.718281828459045235360287471352662497757247093699959574966967627724076630};
template <typename T = coord_t>
inline const T TAU = 2 * PI<T>;
template <typename T = coord_t>
inline const T SQRT2 = T{
    1.414213562373095048801688724209698078569671875376948073176679737990732478};
template <typename T = coord_t>
inline const T ISQRT2 = 1 / SQRT2<T>;
}  // namespace numbers

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
constexpr T ipow(T x, T y) {
    return pow(x, 1 / y);
}

template <typename T>
constexpr T log(T x, T b = numbers::E<T>) {
    return std::log(x) / std::log(b);
}

template <typename T>
constexpr T fmod(T x, T y) {
    return std::fmod(x, y);
}

template <typename T>
constexpr T acos(T x) {
    return std::acos(x);
}

template <typename T>
constexpr T atan2(T y, T x) {
    return std::atan2(y, x);
}

template <typename T>
constexpr T hypot(T x, T y) {
    return std::hypot(x, y);
}

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
constexpr T sgn(T x) {
    return static_cast<T>(static_cast<int>(x > T(0)) -
                          static_cast<int>(T(0) > x));
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
constexpr T remainder(T lhs, T rhs) {
    return ((lhs % rhs) + rhs) % rhs;
}

template <typename T>
constexpr T frac(const T& x) {
    return x - std::floor(x);
}

template <typename Ti, typename Tf>
constexpr Ti floatToWholeFrac(Tf& x) {
    Ti i = static_cast<Ti>(std::floor(x));
    x -= i;
    return i;
}

template <typename T>
constexpr T closerToZero(T x, T y) {
    return abs(x) < abs(y) ? x : y;
}

template <typename T>
constexpr T wrapAngle(T x) {
    x = fmod(x, numbers::TAU<T>);
    if (x > numbers::PI<T>)
        x -= numbers::TAU<T>;
    else if (x < -numbers::PI<T>)
        x += numbers::TAU<T>;
    return x;
}

template <typename T>
constexpr T angleDifference(T x, T y) {
    return wrapAngle(x - y);
}

template <typename T>
constexpr T angleDifferenceAbs(T x, T y) {
    return abs(angleDifference(x, y));
}

template <typename T>
constexpr T angleApproach(T x, T y, T f) {
    x = wrapAngle(x), y = wrapAngle(y);
    coord_t diff = angleDifference(y, x);
    if (diff == 0) return y;
    x = wrapAngle(x + sgn(diff) * f);
    return sgn(angleDifference(y, x)) != sgn(diff) ? y : x;
}
};  // namespace hiemalia

#endif  // M_MATH_HH
