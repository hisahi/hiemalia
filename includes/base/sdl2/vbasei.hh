/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// base/sdl2/vbasei.hh: header file for SDL2 video module implementation

#ifndef M_BASE_SDL2_VBASEI_HH
#define M_BASE_SDL2_VBASEI_HH

#include <memory>

#include "base/sdl2.hh"
#include "base/sdl2/hbasei.hh"
#include "sbuf.hh"
#include "vbase.hh"

namespace hiemalia {
class VideoModuleSDL2 : public VideoModule {
   public:
    std::string name() const noexcept { return name_; }

    void frame();
    void blank();
    void draw(const SplinterBuffer& buffer);
    void blit();
    void sync();

    void onResize();

    explicit VideoModuleSDL2(std::shared_ptr<HostModule> host);
    VideoModuleSDL2(const VideoModuleSDL2& copy) = delete;
    VideoModuleSDL2& operator=(const VideoModuleSDL2& copy) = delete;
    VideoModuleSDL2(VideoModuleSDL2&& move);
    VideoModuleSDL2& operator=(VideoModuleSDL2&& move);
    ~VideoModuleSDL2();

   private:
    static inline const std::string name_ = "VideoModuleSDL2";
    float cx_, cy_, scale_;
    std::shared_ptr<HostModuleSDL2> host_;
    SDL_Window* window_{nullptr};
    SDL_Renderer* renderer_{nullptr};
    SDL_Surface* surface_{nullptr};
    SDL_Texture* screen_{nullptr};
};
};  // namespace hiemalia

#endif  // M_BASE_SDL2_VBASEI_HH
