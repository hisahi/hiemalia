/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// abase.hh: header file for audio module

#ifndef M_ABASE_HH
#define M_ABASE_HH

#include <string>

#include "config.hh"
#include "hbase.hh"
#include "module.hh"

namespace hiemalia {
using sound_t = int;

constexpr int channelAny = -1;
constexpr unsigned int defaultFadeoutDuration = 5000;

class AudioModule : public Module {
  public:
    virtual std::string name() const noexcept = 0;
    std::string role() const noexcept { return role_; }

    virtual void tick() = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;

    // loopCount 0 = infinite, others = play N times
    virtual void playMusic(const std::string& filename, size_t loopCount) = 0;
    virtual void fadeOutMusic(
        unsigned int duration = defaultFadeoutDuration) = 0;
    virtual void stopMusic() = 0;
    virtual bool isMusicPlaying() = 0;

    virtual sound_t loadSound(const std::string& filename) = 0;
    // loopCount 0 = infinite, others = play N times.
    // channel may be channelAny, in which case "play on any free channel"
    virtual void playSound(sound_t soundId, float volume, float pan,
                           size_t loopCount, int channel) = 0;
    virtual void stopSound(int channel) = 0;
    virtual void stopSounds() = 0;

    DELETE_COPY(AudioModule);
    INHERIT_MOVE(AudioModule, Module);
    virtual ~AudioModule() noexcept {}

  protected:
    AudioModule() {}

  private:
    static inline const std::string role_ = "audio module";
};

class AudioModuleNull : public AudioModule {
  public:
    inline std::string name() const noexcept { return name_; }

    inline void tick() {}
    inline void pause() {}
    inline void resume() {}

    inline void playMusic(const std::string& filename, size_t loopCount) {}
    inline void stopMusic() {}
    inline bool isMusicPlaying() { return false; }

    inline sound_t loadSound(const std::string& filename) { return 0; }
    inline void playSound(sound_t soundId, float volume, float pan,
                          size_t loopCount, int channel) {}
    inline virtual void fadeOutMusic(
        unsigned int duration = defaultFadeoutDuration) {}
    inline void stopSound(int channel) {}
    inline void stopSounds() {}

    inline explicit AudioModuleNull(std::shared_ptr<HostModule> host) {}
    inline AudioModuleNull(const AudioModuleNull& copy) = delete;
    inline AudioModuleNull& operator=(const AudioModuleNull& copy) = delete;
    inline AudioModuleNull(AudioModuleNull&& move) noexcept
        : AudioModule(std::move(move)) {}
    inline AudioModuleNull& operator=(AudioModuleNull&& move) noexcept {
        AudioModule::operator=(std::move(move));
        return *this;
    }
    inline ~AudioModuleNull() {}

  private:
    static inline const std::string name_ = "AudioModuleNull";
};

std::shared_ptr<AudioModule> getAudioModule(std::shared_ptr<HostModule> host);

};  // namespace hiemalia

#endif  // M_ABASE_HH
