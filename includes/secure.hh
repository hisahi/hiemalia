/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// secure.hh: header file for "secure" functions

#ifndef M_SECURE_HH
#define M_SECURE_HH

#include <cstdio>
#include <ctime>

namespace hiemalia {
inline std::tm* s_localtime(const std::time_t* time) {
#if _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _BSD_SOURCE || _SVID_SOURCE || \
    _POSIX_SOURCE
    static std::tm buf;
    return localtime_r(time, &buf);
#elif defined(_MSC_VER)
    static std::tm buf;
    size_t e = localtime_s(&buf, time);
    return e ? NULL : &buf;
#else
    return std::localtime(&time);
#endif
}

#if defined(_MSC_VER)
#define s_sscanf sscanf_s
#else
#define s_sscanf std::sscanf
#endif
};  // namespace hiemalia

#endif  // M_SECURE_HH
