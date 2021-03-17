/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// base/sdl2/vbase.cc: implementation of base graphics library on SDL2

#include <algorithm>

#include "base/sdl2.hh"
#include "base/sdl2/hbasei.hh"
#include "base/sdl2/vbasei.hh"
#include "defs.hh"
#include "sbuf.hh"
#include "logger.hh"

namespace hiemalia {
static int int_log2(int x) {
    x &= ~(x >> 1);
    x &= ~(x >> 2);
    x &= ~(x >> 4);
    x &= ~(x >> 8);
    if constexpr (sizeof(x) > 2) x &= ~(x >> 16);
    if constexpr (sizeof(x) > 4) x &= ~(x >> 32);
    return x;
}

VideoModuleSDL2::VideoModuleSDL2(std::shared_ptr<HostModule> host)
    : host_(std::move(std::dynamic_pointer_cast<HostModuleSDL2>(host))) {
    dynamic_assert(host_ != nullptr, "must be HostModuleSDL2!!!");
    if (SDL_InitSubSystem(SDL_INIT_VIDEO))
        throw SDLException("could not initialize SDL2 video subsystem: " +
                           std::string(SDL_GetError()));
    SDL_DisplayMode current;
    int maxSize = 512;
    if (!SDL_GetCurrentDisplayMode(0, &current))
        maxSize = int_log2(std::min(current.w - 200, current.h - 200));
    logger.info_fmt("setting initial window size to %dx%d", maxSize, maxSize);
    if (!(window_ = SDL_CreateWindow(gameTitle.c_str(), SDL_WINDOWPOS_UNDEFINED,
                                     SDL_WINDOWPOS_UNDEFINED, maxSize, maxSize,
                                     SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE)))
        throw SDLException("Could not initialize SDL2 window: " +
                           std::string(SDL_GetError()));
    if (!(renderer_ = SDL_CreateRenderer(
              window_, -1,
              SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)))
        throw SDLException("Could not initialize SDL2 renderer: " +
                           std::string(SDL_GetError()));
    onResize();
    host_->addVideoModule(*this);
}

VideoModuleSDL2::VideoModuleSDL2(VideoModuleSDL2 &&move)
    : VideoModule(std::move(move)), host_(std::move(move.host_)) {
    std::swap(window_, move.window_);
    std::swap(renderer_, move.renderer_);
    onResize();
    host_->addVideoModule(*this);
}

VideoModuleSDL2 &VideoModuleSDL2::operator=(VideoModuleSDL2 &&move) {
    VideoModule::operator=(std::move(move));
    std::swap(window_, move.window_);
    std::swap(renderer_, move.renderer_);
    onResize();
    return *this;
}

VideoModuleSDL2::~VideoModuleSDL2() {
    host_->removeVideoModule(*this);
    if (renderer_) SDL_DestroyRenderer(renderer_);
    if (window_) SDL_DestroyWindow(window_);
}

void VideoModuleSDL2::onResize() {
    int w, h;
    SDL_GetRendererOutputSize(renderer_, &w, &h);
    cx_ = w * 0.5f;
    cy_ = h * 0.5f;
    scale_ = std::min(cx_, cy_);
}

void VideoModuleSDL2::frame() {}

void VideoModuleSDL2::blank() {
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer_);
}

void VideoModuleSDL2::draw(const SplinterBuffer &buffer) {
    for (auto &s : buffer) {
        SDL_SetRenderDrawColor(renderer_, s.r, s.g, s.b, s.a);
        SDL_RenderDrawLine(renderer_, static_cast<int>(s.x1),
                           static_cast<int>(s.y1), static_cast<int>(s.x2),
                           static_cast<int>(s.y2));
    }
}

void VideoModuleSDL2::blit() { SDL_RenderPresent(renderer_); }

void VideoModuleSDL2::sync() { host_->sync(); }

}  // namespace hiemalia
