/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// base/sdl2/vbasei.cc: implementation of base graphics library on SDL2

#include <algorithm>

#include "base/sdl2.hh"
#include "base/sdl2/hbasei.hh"
#include "base/sdl2/vbasei.hh"
#include "defs.hh"
#include "logger.hh"
#include "sbuf.hh"

namespace hiemalia {
static int round_down(int x) { return (x / 256) * 256; }

VideoModuleSDL2::VideoModuleSDL2(std::shared_ptr<HostModule> host)
    : host_(std::move(std::dynamic_pointer_cast<HostModuleSDL2>(host))) {
    dynamic_assert(host_ != nullptr, "must be HostModuleSDL2!!!");
    if (SDL_InitSubSystem(SDL_INIT_VIDEO))
        throw SDLException("could not initialize SDL2 video subsystem");
    SDL_DisplayMode current;
    int maxSize = 512;
    if (!SDL_GetCurrentDisplayMode(0, &current))
        maxSize = round_down(std::min(current.w - 200, current.h - 200));
    LOG_INFO("setting initial window size to %dx%d", maxSize, maxSize);
    if (!(window_ = SDL_CreateWindow(gameTitle.c_str(), SDL_WINDOWPOS_UNDEFINED,
                                     SDL_WINDOWPOS_UNDEFINED, maxSize, maxSize,
                                     SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE)))
        throw SDLException("Could not initialize SDL2 window");
    if (!(renderer_ = SDL_CreateRenderer(
              window_, -1,
              SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)))
        throw SDLException("Could not initialize SDL2 renderer");
    if (SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_ADD))
        LOG_WARN("alpha not available");
    onResize();
    host_->addVideoModule(*this);
}

VideoModuleSDL2::VideoModuleSDL2(VideoModuleSDL2 &&move) noexcept
    : VideoModule(std::move(move)), host_(std::move(move.host_)) {
    std::swap(window_, move.window_);
    std::swap(renderer_, move.renderer_);
    onResize();
    host_->addVideoModule(*this);
}

VideoModuleSDL2 &VideoModuleSDL2::operator=(VideoModuleSDL2 &&move) noexcept {
    VideoModule::operator=(std::move(move));
    if (host_ != move.host_) {
        host_->removeVideoModule(*this);
        host_ = move.host_;
        host_->addVideoModule(*this);
    }
    std::swap(window_, move.window_);
    std::swap(renderer_, move.renderer_);
    onResize();
    return *this;
}

VideoModuleSDL2::~VideoModuleSDL2() noexcept {
    host_->removeVideoModule(*this);
    if (renderer_) SDL_DestroyRenderer(renderer_);
    if (window_) SDL_DestroyWindow(window_);
}

void VideoModuleSDL2::onResize() {
    int w, h;
    SDL_GetRendererOutputSize(renderer_, &w, &h);
    width_ = w;
    cx_ = w * 0.5f;
    cy_ = h * 0.5f;
    scale_ = std::min(cx_, cy_);
}

void VideoModuleSDL2::frame() {}

void VideoModuleSDL2::blank() {
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer_);
    SDL_RenderSetClipRect(renderer_, NULL);
}

void VideoModuleSDL2::draw(const SplinterBuffer &buffer) {
    int x, y;
    for (auto &s : buffer) {
        x = static_cast<int>(s.x * scale_ + cx_);
        y = static_cast<int>(s.y * scale_ + cy_);
        switch (s.type) {
            case SplinterType::BeginShape:
                SDL_SetRenderDrawColor(renderer_, s.color.r, s.color.g,
                                       s.color.b, s.color.a);
                points_.clear();
                points_.push_back(SDL_Point{x, y});
                break;
            case SplinterType::Point:
                points_.push_back(SDL_Point{x, y});
                break;
            case SplinterType::EndShapePoint:
                points_.push_back(SDL_Point{x, y});
                SDL_RenderDrawLines(renderer_, points_.data(), points_.size());
                break;
            case SplinterType::BeginClipCenter:
                rect_.x = 0;
                rect_.y = x;
                rect_.w = width_;
                rect_.h = y - x;
                SDL_RenderSetClipRect(renderer_, &rect_);
                break;
            case SplinterType::EndClip:
                SDL_RenderSetClipRect(renderer_, NULL);
                break;
        }
    }
}

void VideoModuleSDL2::blit() { SDL_RenderPresent(renderer_); }

void VideoModuleSDL2::sync() { host_->sync(); }

}  // namespace hiemalia
