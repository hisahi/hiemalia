/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// mholder.cc: implementation for the module holder

#include "mholder.hh"

#include "abase.hh"
#include "assets.hh"
#include "hbase.hh"
#include "ibase.hh"
#include "logger.hh"
#include "logic.hh"
#include "menumain.hh"
#include "vbase.hh"

namespace hiemalia {

ModuleHolder::ModuleHolder(const std::shared_ptr<HostModule> host,
                           GameState &state)
    : input(Module::create_unique<InputEngine>(host, state)),
      video(Module::create_unique<VideoEngine>(host, state)),
      audio(Module::create_unique<AudioEngine>(host, state)),
      logic(Module::create_unique<LogicEngine>()) {}

ModuleHolder::ModuleHolder(ModuleHolder &&move) noexcept
    : input(std::move(move.input)),
      video(std::move(move.video)),
      audio(std::move(move.audio)),
      logic(std::move(move.logic)) {}

ModuleHolder &ModuleHolder::operator=(ModuleHolder &&move) noexcept {
    input = std::move(move.input);
    video = std::move(move.video);
    audio = std::move(move.audio);
    logic = std::move(move.logic);
    return *this;
}

void ModuleHolder::loadAssets() { audio->load(); }

}  // namespace hiemalia
