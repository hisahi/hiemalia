/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// audio.hh: header file for audio engine module

#ifndef M_AUDIO_HH
#define M_AUDIO_HH

#include <variant>

#include "abase.hh"
#include "config.hh"
#include "inherit.hh"
#include "math.hh"
#include "model.hh"
#include "module.hh"
#include "msg.hh"
#include "sbuf.hh"
#include "sounds.hh"
#include "state.hh"

namespace hiemalia {
enum class AudioMessageType {
    PlaySound,
    StopSounds,
    PlayMusic,
    FadeOutMusic,
    StopMusic,
    Pause,
    Resume,
    Mute,
    Unmute
};

struct AudioMessageSoundEffect {
    SoundEffect sound;
    float volume;
    float pan;
    float pitch;
};

struct AudioMessage {
    AudioMessageType type;

    inline static AudioMessage playSound(SoundEffect sound, float volume = 1.0f,
                                         float pan = 0.0f, float pitch = 1.0f) {
        return AudioMessage(AudioMessageType::PlaySound,
                            AudioMessageSoundEffect{sound, volume, pan, pitch});
    }
    inline static AudioMessage playSound(SoundEffect sound, Point3D relative) {
        return AudioMessage(
            AudioMessageType::PlaySound,
            AudioMessageSoundEffect{
                sound,
                clamp(0.5f, static_cast<float>(4.0 / relative.length()), 1.0f),
                static_cast<float>(relative.normalize().x * 0.5), 1.0f});
    }
    inline static AudioMessage playMusic(MusicTrack track) {
        return AudioMessage(AudioMessageType::PlayMusic, track);
    }
    inline static AudioMessage stopSounds() {
        return AudioMessage(AudioMessageType::StopSounds);
    }
    inline static AudioMessage fadeOutMusic() {
        return AudioMessage(AudioMessageType::FadeOutMusic);
    }
    inline static AudioMessage stopMusic() {
        return AudioMessage(AudioMessageType::StopMusic);
    }
    inline static AudioMessage pause() {
        return AudioMessage(AudioMessageType::Pause);
    }
    inline static AudioMessage resume() {
        return AudioMessage(AudioMessageType::Resume);
    }
    inline static AudioMessage mute() {
        return AudioMessage(AudioMessageType::Mute);
    }
    inline static AudioMessage unmute() {
        return AudioMessage(AudioMessageType::Unmute);
    }

    inline MusicTrack getMusic() const { return std::get<MusicTrack>(x); }
    inline AudioMessageSoundEffect getSound() const {
        return std::get<AudioMessageSoundEffect>(x);
    }

  private:
    AudioMessage(AudioMessageType t) : type(t) {}
    AudioMessage(AudioMessageType t, AudioMessageSoundEffect sound)
        : type(t), x(sound) {
        dynamic_assert(type == AudioMessageType::PlaySound, "invalid init");
    }
    AudioMessage(AudioMessageType t, MusicTrack music) : type(t), x(music) {
        dynamic_assert(type == AudioMessageType::PlayMusic, "invalid init");
    }
    std::variant<AudioMessageSoundEffect, MusicTrack> x;
};

class AudioConfig : public ConfigSection {
  public:
    AudioConfig() noexcept : ConfigSection("Audio") {}

    void load(ConfigSectionStore store) override;
    void save(ConfigSectionStore store) const override;

    bool music{true};
    bool sound{true};
};

class AudioEngine : public Module, MessageHandler<AudioMessage> {
  public:
    std::string name() const noexcept { return name_; }
    std::string role() const noexcept { return role_; }

    DELETE_COPY(AudioEngine);

    AudioEngine(AudioEngine&& move) noexcept
        : Module(std::move(move)), audio_(std::move(move.audio_)) {}
    AudioEngine& operator=(AudioEngine&& move) noexcept {
        Module::operator=(std::move(move));
        audio_ = std::move(move.audio_);
        return *this;
    }
    AudioEngine(std::shared_ptr<HostModule> host, GameState& state);
    ~AudioEngine() noexcept {}

    void load();
    void tick();
    void gotMessage(const AudioMessage& msg);
    inline const ConfigSectionPtr<AudioConfig>& getConfig() { return config_; }

    void mute();
    void unmute();
    bool isMuted() const;

    bool canPlayMusic();
    bool canPlaySound();

  private:
    std::shared_ptr<AudioModule> audio_;
    ConfigSectionPtr<AudioConfig> config_;
    bool muted_{false};
    static inline const std::string name_ = "AudioEngine";
    static inline const std::string role_ = "audio engine";
};

}  // namespace hiemalia

#endif  // M_AUDIO_HH
