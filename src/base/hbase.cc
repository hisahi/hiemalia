/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// base/hbase.cc: implementation of host module getter

#include <exception>
#include <memory>
#include <stdexcept>

#include "basemacr.hh"
#include "defs.hh"
#include "logger.hh"

// host backends
#include "hbase.hh"
#ifdef HBACKEND_sdl2
#include "base/sdl2/hbasei.hh"
#endif

namespace hiemalia {

std::shared_ptr<HostModule> getHostModule() {
#ifdef HBACKEND_sdl2
    TRY_MODULE("host", HostModuleSDL2);
#endif
    never("no available host module!");
}

}  // namespace hiemalia
