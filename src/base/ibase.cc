/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// base/ibase.cc: implementation of input module getter

#include <exception>
#include <memory>
#include <stdexcept>

#include "basemacr.hh"
#include "defs.hh"
#include "hbase.hh"
#include "logger.hh"

// input backends
#include "ibase.hh"
#ifdef IBACKEND_sdl2
#include "base/sdl2/ibasei.hh"
#endif

namespace hiemalia {

std::shared_ptr<InputModule> getInputModule(std::shared_ptr<HostModule> host) {
#ifdef IBACKEND_sdl2
    TRY_MODULE("input", InputModuleSDL2, std::move(host));
#endif
    never("no available input module!");
}

}  // namespace hiemalia
