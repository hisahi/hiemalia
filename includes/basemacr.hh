/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// basemacr.hh: header file for macros used by *base.cc and *base.cc only

#ifndef M_BASEMACR_HH
#define M_BASEMACR_HH

#include <stdexcept>

#include "logger.hh"

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)
#define MAKE_MODULE(M, ...) std::move(Module::create<M>(__VA_ARGS__))
#define TRY_MODULE(T, M, ...)                                      \
    do {                                                           \
        try {                                                      \
            return MAKE_MODULE(M, __VA_ARGS__);                    \
        } catch (const std::runtime_error& e) {                    \
            LOG_WARN(std::string("could not initialize " T         \
                                 " module '" STRINGIFY(M) "': ") + \
                     e.what());                                    \
        }                                                          \
    } while (0);

#endif  // M_BASEMACR_HH
