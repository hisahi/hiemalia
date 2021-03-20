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

class AudioModule : public Module {
   public:
    virtual std::string name() const noexcept = 0;
    std::string role() const noexcept { return role_; }

    virtual void tick() = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;

    // loopCount 0 = infinite, others = play N times
    virtual void playMusic(const std::string& filename, size_t loopCount) = 0;
    virtual void stopMusic() = 0;
    virtual bool isMusicPlaying() = 0;

    // loopCount 0 = infinite, others = play N times
    virtual sound_t loadSound(const std::string& filename) = 0;
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
    std::string name() const noexcept { return name_; }

    void tick() {}
    void pause() {}
    void resume() {}

    void playMusic(const std::string& filename, size_t loopCount) {}
    void stopMusic() {}
    bool isMusicPlaying() { return false; }

    sound_t loadSound(const std::string& filename) { return 0; }
    void playSound(sound_t soundId, float volume, float pan, size_t loopCount,
                   int channel) {}
    void stopSound(int channel) {}
    void stopSounds() {}

    explicit AudioModuleNull(std::shared_ptr<HostModule> host) {}
    AudioModuleNull(const AudioModuleNull& copy) = delete;
    AudioModuleNull& operator=(const AudioModuleNull& copy) = delete;
    AudioModuleNull(AudioModuleNull&& move) : AudioModule(std::move(move)) {}
    AudioModuleNull& operator=(AudioModuleNull&& move) {
        AudioModule::operator=(std::move(move));
        return *this;
    }
    ~AudioModuleNull() {}

   private:
    static inline const std::string name_ = "AudioModuleNull";
};

std::shared_ptr<AudioModule> getAudioModule(std::shared_ptr<HostModule> host);

};  // namespace hiemalia

#endif  // M_ABASE_HH
