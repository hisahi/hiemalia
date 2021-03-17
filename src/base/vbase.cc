/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// base/vbase.cc: implementation of video module getter

#include <exception>
#include <memory>
#include <stdexcept>

#include "basemacr.hh"
#include "defs.hh"
#include "hbase.hh"
#include "logger.hh"

// video backends
#include "vbase.hh"
#ifdef VBACKEND_sdl2
#include "base/sdl2/vbasei.hh"
#endif

namespace hiemalia {

std::shared_ptr<VideoModule> getVideoModule(std::shared_ptr<HostModule> host) {
#ifdef VBACKEND_sdl2
    TRY_MODULE("video", VideoModuleSDL2, std::move(host));
#endif
    never("no available video module!");
}

}  // namespace hiemalia
