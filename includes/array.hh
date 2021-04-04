/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// array.hh: array includes

#ifndef M_ARRAY_HH
#define M_ARRAY_HH

#include <algorithm>
#include <array>
#include <string_view>
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

template <std::size_t N>
constexpr auto one_short_(const char (&s)[N + 1]) -> std::array<char, N> {
    std::array<char, N> arr{};
    for (std::size_t i = 0; i < N; ++i) arr[i] = s[i];
    return arr;
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

template <std::size_t N>
constexpr auto str_to_array(const char (&s)[N]) {
    return one_short_<N - 1>(s);
}

template <typename T, size_t N>
constexpr bool arrayContains(const std::array<T, N>& arr, const T& val) {
    return std::find(arr.begin(), arr.end(), val) != arr.end();
}
}  // namespace hiemalia

#endif  // M_ARRAY_HH
