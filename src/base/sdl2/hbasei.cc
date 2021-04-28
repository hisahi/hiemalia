/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// base/sdl2/hbasei.cc: implementation of HostModuleSDL2

#include "base/sdl2/hbasei.hh"

#include <algorithm>

#include "base/sdl2.hh"
#include "base/sdl2/ibasei.hh"
#include "base/sdl2/vbasei.hh"
#include "helpers.hh"
#include "hiemalia.hh"
#include "logger.hh"

namespace hiemalia {

hiemalia::HostModuleSDL2::HostModuleSDL2() {
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENTS))
        throw SDLException("Could not initialize SDL2");
    sdl_owner_ = true;
    ticks_ = SDL_GetTicks();
}

HostModuleSDL2::~HostModuleSDL2() noexcept {
    if (sdl_owner_) SDL_Quit();
}

HostModuleSDL2::HostModuleSDL2(HostModuleSDL2&& move) noexcept
    : HostModule(std::move(move)) {
    std::swap(sdl_owner_, move.sdl_owner_);
    std::swap(ticks_, move.ticks_);
    std::swap(quit_, move.quit_);
}

HostModuleSDL2& HostModuleSDL2::operator=(HostModuleSDL2&& move) noexcept {
    HostModule::operator=(std::move(move));
    std::swap(sdl_owner_, move.sdl_owner_);
    std::swap(ticks_, move.ticks_);
    std::swap(quit_, move.quit_);
    return *this;
}

void HostModuleSDL2::begin() {}

bool HostModuleSDL2::proceed() {
    if (quit_) return false;
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                LOG_DEBUG("Got quit command from SDL");
                quit_ = true;
                break;
            case SDL_KEYDOWN:
                if (arcade_ && event.key.keysym.sym == SDLK_F4 &&
                    (SDL_GetModState() & KMOD_ALT)) {
                    LOG_DEBUG("arcade quit SDL Alt+F4!");
                    quit_ = true;
                }
                if (arcade_ && event.key.keysym.sym == SDLK_F1) {
                    LOG_DEBUG("coin");
                    hiemalia_tryAddCredits(1);
                }
                [[fallthrough]];
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

void HostModuleSDL2::removeVideoModule(VideoModuleSDL2& module) noexcept {
    eraseRemove(video_modules_, &module);
}

void HostModuleSDL2::addAudioModule(AudioModuleSDL2& module) {
    audio_modules_.push_back(&module);
}

void HostModuleSDL2::removeAudioModule(AudioModuleSDL2& module) noexcept {
    eraseRemove(audio_modules_, &module);
}

void HostModuleSDL2::addInputModule(InputModuleSDL2& module) {
    input_modules_.push_back(&module);
}

void HostModuleSDL2::removeInputModule(InputModuleSDL2& module) noexcept {
    eraseRemove(input_modules_, &module);
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
    LOG_DEBUG("Got quit command from game");
    quit_ = true;
}

void HostModuleSDL2::finish() { quit_ = true; }

void HostModuleSDL2::arcade() {
    LOG_DEBUG("Entering arcade mode");
    arcade_ = true;
}

void hostDisplayError(const std::string& title, const std::string& text) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title.c_str(), text.c_str(),
                             nullptr);
}
}  // namespace hiemalia
