/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// base/sdl2/abasei.hh: header file for SDL(_mixer) 2 audio module impl

#ifndef M_BASE_SDL2_ABASEI_HH
#define M_BASE_SDL2_ABASEI_HH

#include <memory>
#include <vector>

#include "abase.hh"
#include "base/sdl2.hh"
#include "base/sdl2/hbasei.hh"
#include "defs.hh"
#include "inherit.hh"

namespace hiemalia {
struct SDLSoundClip {
    SDLSoundClip(Mix_Chunk* chunk) : sdl(chunk) {}

    DELETE_COPY(SDLSoundClip);
    SDLSoundClip(SDLSoundClip&& move) noexcept : sdl(move.sdl) {
        move.sdl = nullptr;
    }
    SDLSoundClip& operator=(SDLSoundClip&& move) noexcept {
        if (sdl != move.sdl) {
            if (sdl) Mix_FreeChunk(sdl);
            sdl = move.sdl;
        }
        return *this;
    }

    ~SDLSoundClip() {
        if (sdl) Mix_FreeChunk(sdl);
    }

    Mix_Chunk* sdl;
};

struct SDLMixer {
    SDLMixer();
    ~SDLMixer() noexcept;
    SDLMixer(SDLMixer&& move) noexcept;
    SDLMixer& operator=(SDLMixer&& move) noexcept;
    DELETE_COPY(SDLMixer);

  private:
    bool own_{false};
};

class AudioModuleSDLMixer2 : public AudioModule {
  public:
    std::string name() const noexcept { return name_; }

    inline bool canPlayMusic() { return true; }
    inline bool canPlaySound() { return true; }

    void tick();
    void pause();
    void resume();

    void playMusic(const std::string& filename, size_t loopCount);
    void fadeOutMusic(unsigned int duration = defaultFadeoutDuration);
    void stopMusic();
    bool isMusicPlaying();

    sound_t loadSound(const std::string& filename);
    void playSound(sound_t soundId, float volume, float pan, float pitch,
                   size_t loopCount, int channel);
    void stopSound(int channel);
    void stopSounds();

    explicit AudioModuleSDLMixer2(std::shared_ptr<HostModule> host);
    DELETE_COPY(AudioModuleSDLMixer2);
    AudioModuleSDLMixer2(AudioModuleSDLMixer2&& move) noexcept;
    AudioModuleSDLMixer2& operator=(AudioModuleSDLMixer2&& move) noexcept;
    ~AudioModuleSDLMixer2() noexcept;

  private:
    static inline const std::string name_ = "AudioModuleSDLMixer2";
    Mix_Music* music_{nullptr};
    std::shared_ptr<HostModuleSDL2> host_;
    std::unique_ptr<SDLMixer> mixer_;
    std::vector<SDLSoundClip> sounds_;
};
};  // namespace hiemalia

#endif  // M_BASE_SDL2_ABASEI_HH
