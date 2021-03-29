/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// inherit.hh: inheritance helpers

#ifndef M_INHERIT_HH
#define M_INHERIT_HH

#include <algorithm>
#include <type_traits>

namespace hiemalia {

#define DELETE_COPY(T)         \
    T(const T& copy) = delete; \
    T& operator=(const T& copy) = delete;

#define INHERIT_COPY(T, Tbase)                                             \
    T(const T& copy)                                                       \
    noexcept(std::is_nothrow_copy_constructible_v<Tbase>) : Tbase(copy) {} \
    T& operator=(const T& copy) noexcept(                                  \
        std::is_nothrow_copy_assignable_v<Tbase>) {                        \
        Tbase::operator=(copy);                                            \
        return *this;                                                      \
    }

#define DELETE_MOVE(T)    \
    T(T&& move) = delete; \
    T& operator=(T&& move) = delete;

#define INHERIT_MOVE(T, Tbase)                            \
    T(T&& move)                                           \
    noexcept(std::is_nothrow_move_constructible_v<Tbase>) \
        : Tbase(std::move(move)) {}                       \
    T& operator=(T&& move) noexcept(                      \
        std::is_nothrow_move_assignable_v<Tbase>) {       \
        Tbase::operator=(std::move(move));                \
        return *this;                                     \
    }

}  // namespace hiemalia

#endif  // M_INHERIT_HH
