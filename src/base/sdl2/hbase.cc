/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// base/sdl2/hbase.cc: implementation of HostModuleSDL2

#include <algorithm>

#include "base/sdl2.hh"
#include "base/sdl2/hbasei.hh"
#include "base/sdl2/ibasei.hh"
#include "base/sdl2/vbasei.hh"
#include "helpers.hh"
#include "hiemalia.hh"
#include "logger.hh"

namespace hiemalia {

hiemalia::HostModuleSDL2::HostModuleSDL2() {
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENTS))
        throw SDLException(SDL_GetError());
    sdl_owner_ = true;
    ticks_ = SDL_GetTicks();
}

HostModuleSDL2::~HostModuleSDL2() {
    if (sdl_owner_) SDL_Quit();
}

HostModuleSDL2::HostModuleSDL2(HostModuleSDL2&& move)
    : HostModule(std::move(move)) {
    std::swap(sdl_owner_, move.sdl_owner_);
    std::swap(ticks_, move.ticks_);
    std::swap(quit_, move.quit_);
}

HostModuleSDL2& HostModuleSDL2::operator=(HostModuleSDL2&& move) {
    HostModule::operator=(std::move(move));
    std::swap(sdl_owner_, move.sdl_owner_);
    std::swap(ticks_, move.ticks_);
    std::swap(quit_, move.quit_);
    return *this;
}

void HostModuleSDL2::begin() {}

bool HostModuleSDL2::proceed() {
    if (quit_) return true;
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                logger.debug("Got quit command from SDL");
                quit_ = true;
                break;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP:
                for (auto* module : input_modules_) module->handle(event);
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                    for (auto* module : video_modules_) module->onResize();
                break;
        }
    }
    return !quit_;
}

void HostModuleSDL2::addVideoModule(VideoModuleSDL2& module) {
    video_modules_.push_back(&module);
}

void HostModuleSDL2::removeVideoModule(VideoModuleSDL2& module) {
    erase_remove(video_modules_, &module);
}

void HostModuleSDL2::addAudioModule(AudioModuleSDL2& module) {
    audio_modules_.push_back(&module);
}

void HostModuleSDL2::removeAudioModule(AudioModuleSDL2& module) {
    erase_remove(audio_modules_, &module);
}

void HostModuleSDL2::addInputModule(InputModuleSDL2& module) {
    input_modules_.push_back(&module);
}

void HostModuleSDL2::removeInputModule(InputModuleSDL2& module) {
    erase_remove(input_modules_, &module);
}

void HostModuleSDL2::sync() {
    static unsigned int new_ticks;
    while (frac_ < tickMicroseconds) {
        new_ticks = SDL_GetTicks();
        frac_ += 1000ULL * (new_ticks - ticks_);
        ticks_ = new_ticks;
        SDL_Delay(2);
    }
    frac_ %= tickMicroseconds;
}

void HostModuleSDL2::quit() {
    logger.debug("Got quit command from game");
    quit_ = true;
}

void HostModuleSDL2::finish() { quit(); }
}  // namespace hiemalia
