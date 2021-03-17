/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// audio.hh: header file for audio engine module

#ifndef M_AUDIO_HH
#define M_AUDIO_HH

#include "abase.hh"
#include "config.hh"
#include "module.hh"
#include "msg.hh"
#include "sbuf.hh"

namespace hiemalia {
enum class AudioMessageType { PlaySound, StopSounds };

struct AudioMessage {
    AudioMessageType type;

   private:
    AudioMessage(AudioMessageType t) : type(t) {}
};

class AudioEngine : public Module, MessageHandler<AudioMessage> {
   public:
    std::string name() const noexcept { return name_; }
    std::string role() const noexcept { return role_; }

    AudioEngine(const AudioEngine& copy) = delete;
    AudioEngine& operator=(const AudioEngine& copy) = delete;
    AudioEngine(AudioEngine&& move)
        : Module(std::move(move)), audio_(std::move(move.audio_)) {}
    AudioEngine& operator=(AudioEngine&& move) {
        Module::operator=(std::move(move));
        audio_ = std::move(move.audio_);
        return *this;
    }
    AudioEngine(std::shared_ptr<HostModule> host);
    virtual ~AudioEngine() {}

    void load();
    void tick();
    void gotMessage(const AudioMessage& msg);

   private:
    std::shared_ptr<AudioModule> audio_;
    static inline const std::string name_ = "AudioEngine";
    static inline const std::string role_ = "audio engine";
};

}  // namespace hiemalia

#endif  // M_AUDIO_HH
