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

class AudioModule : public Module {
   public:
    virtual std::string name() const noexcept = 0;
    std::string role() const noexcept { return role_; }
    virtual void tick() = 0;

    virtual void playMusic(sound_t trackId, size_t loopCount) = 0;
    virtual void stopMusic() = 0;

    virtual void playSound(sound_t soundId, float volume, float pan,
                           size_t loopCount, int channel) = 0;
    virtual void stopSound(int channel) = 0;
    virtual void stopSounds() = 0;

    virtual void pause() = 0;
    virtual void resume() = 0;

    virtual sound_t loadMusic(const std::string& filename) = 0;
    virtual sound_t loadSound(const std::string& filename) = 0;
    AudioModule(const AudioModule& copy) = delete;
    AudioModule& operator=(const AudioModule& copy) = delete;
    AudioModule(AudioModule&& move) : Module(std::move(move)) {}
    AudioModule& operator=(AudioModule&& move) {
        Module::operator=(std::move(move));
        return *this;
    }
    virtual ~AudioModule() {}

   protected:
    AudioModule() {}

   private:
    static inline const std::string role_ = "audio module";
};

class AudioModuleNull : public AudioModule {
   public:
    std::string name() const noexcept { return name_; }
    void tick() {}

    void playMusic(sound_t trackId, size_t loopCount) {}
    void stopMusic() {}

    void playSound(sound_t soundId, float volume, float pan, size_t loopCount,
                   int channel) {}
    void stopSound(int channel) {}
    void stopSounds() {}

    void pause() {}
    void resume() {}

    sound_t loadMusic(const std::string& filename) { return 0; }
    sound_t loadSound(const std::string& filename) { return 0; }

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
