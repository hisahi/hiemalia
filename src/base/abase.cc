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
#ifdef ABACKEND_SDLMIXER2
#include "base/sdlmixer/abasei.hh"
#endif

namespace hiemalia {

std::shared_ptr<AudioModule> getAudioModule(std::shared_ptr<HostModule> host) {
#ifdef ABACKEND_SDLMIXER2
    TRY_MODULE("audio", AudioModuleSDLMixer2, std::move(host));
#endif
    logger.warn("Falling back to silent audio module");
    return MAKE_MODULE(AudioModuleNull, host);
}

}  // namespace hiemalia
