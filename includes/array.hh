/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// array.hh: array includes

#ifndef M_ARRAY_HH
#define M_ARRAY_HH

#include <array>
#include <type_traits>

namespace hiemalia {
// std::to_array in C++20

namespace {
template <typename T, std::size_t N, std::size_t... I>
constexpr std::array<std::remove_cv_t<T>, N> to_array_(
    T (&arr)[N], std::index_sequence<I...>) {
    return {{std::move(arr[I])...}};
}

template <typename T, std::size_t N, std::size_t... I>
constexpr std::array<std::remove_cv_t<T>, N> to_array_(
    T(&&arr)[N], std::index_sequence<I...>) {
    return {{std::move(arr[I])...}};
}
}  // namespace

template <typename T, std::size_t N>
constexpr std::array<std::remove_cv_t<T>, N> to_array(T (&arr)[N]) {
    return to_array_(arr, std::make_index_sequence<N>{});
}

template <typename T, std::size_t N>
constexpr std::array<std::remove_cv_t<T>, N> to_array(T(&&arr)[N]) {
    return to_array_(std::move(arr), std::make_index_sequence<N>{});
}
}  // namespace hiemalia

#endif  // M_ARRAY_HH
