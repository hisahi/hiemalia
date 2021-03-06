/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// base/abase.cc: implementation of audio module getter

#include <exception>
#include <memory>
#include <stdexcept>

#include "basemacr.hh"
#include "defs.hh"
#include "hbase.hh"
#include "logger.hh"

// audio backends
#include "abase.hh"
#ifdef ABACKEND_sdl2
#include "base/sdl2/abasei.hh"
#endif

namespace hiemalia {

std::shared_ptr<AudioModule> getAudioModule(
    const std::shared_ptr<HostModule>& host) {
#ifdef ABACKEND_sdl2
    TRY_MODULE("audio", AudioModuleSDLMixer2, std::move(host));
#endif
    LOG_WARN("Falling back to silent audio module");
    return MAKE_MODULE(AudioModuleNull, host);
}

}  // namespace hiemalia
