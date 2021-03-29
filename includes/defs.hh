/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// defs.hh: useful definitions

#ifndef M_DEFS_HH
#define M_DEFS_HH

#include <memory>
#include <string>
#include <vector>

#ifndef NDEBUG
#define NDEBUG 0
#endif

namespace hiemalia {
static inline const std::string gameVersion = "v0.3";
static inline const std::string gameTitle = "Hiemalia " + gameVersion;

using coord_t = double;
#define FMT_coord_t "%lf"

constexpr unsigned tickCount = 60;
constexpr unsigned long long tickMicroseconds = 1000000ULL / tickCount;
constexpr float tickInterval = 1.f / tickCount;

void dynamic_assert_(const std::string &file, unsigned line, bool condition,
                     const std::string &msg);
[[noreturn]] void never_(const std::string &file, unsigned line,
                         const std::string &msg);
};  // namespace hiemalia

#define never(msg) hiemalia::never_(__FILE__, __LINE__, msg)
#if NDEBUG
#define dynamic_assert(cond, msg)
#else
#define dynamic_assert(cond, msg) \
    hiemalia::dynamic_assert_(__FILE__, __LINE__, cond, msg)
#endif

#if !NDEBUG
#include "debugger.hh"
#endif

#endif  // M_DEFS_HH
