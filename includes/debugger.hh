/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// debugger.hh: definition of debugger()

#ifndef M_DEBUGGER_HH
#define M_DEBUGGER_HH

#include "defs.hh"

#if NDEBUG
#define debugger()
#elif defined(_MSC_VER)
#define debugger() __debugbreak()
#elif defined(__unix__)
#include <signal.h>
#include <unistd.h>
#define debugger() raise(SIGTRAP)
#else
#pragma message "warning: debugger calls not supported"
#define debugger()
#endif

#endif  // M_DEBUGGER_HH
