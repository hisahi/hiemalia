/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// base/sdl2/hbasei.hh: header file for SDL2 host module implementation

#ifndef M_BASE_SDL2_HBASEI_HH
#define M_BASE_SDL2_HBASEI_HH

#include <memory>
#include <string>
#include <vector>

#include "abase.hh"
#include "base/sdl2.hh"
#include "hbase.hh"
#include "ibase.hh"
#include "vbase.hh"

namespace hiemalia {

class VideoModuleSDL2;
class AudioModuleSDL2;
class InputModuleSDL2;

class HostModuleSDL2 : public HostModule {
   public:
    std::string name() const noexcept { return name_; }
    void begin();
    bool proceed();
    void quit();
    void finish();

    void sync();

    void addVideoModule(VideoModuleSDL2& module);
    void removeVideoModule(VideoModuleSDL2& module);
    void addAudioModule(AudioModuleSDL2& module);
    void removeAudioModule(AudioModuleSDL2& module);
    void addInputModule(InputModuleSDL2& module);
    void removeInputModule(InputModuleSDL2& module);

    explicit HostModuleSDL2();
    HostModuleSDL2(const HostModuleSDL2& copy) = delete;
    HostModuleSDL2& operator=(const HostModuleSDL2& copy) = delete;
    HostModuleSDL2(HostModuleSDL2&& move);
    HostModuleSDL2& operator=(HostModuleSDL2&& move);
    ~HostModuleSDL2();

   private:
    static inline const std::string name_ = "HostModuleSDL2";
    std::vector<VideoModuleSDL2*> video_modules_;
    std::vector<AudioModuleSDL2*> audio_modules_;
    std::vector<InputModuleSDL2*> input_modules_;
    bool sdl_owner_{false};
    unsigned int ticks_{0};
    unsigned int frac_{0};
    bool quit_{false};
};
};  // namespace hiemalia

#endif  // M_BASE_SDL2_HBASEI_HH
