/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// audio.cc: implementation of AudioEngine

#include "audio.hh"

#include "abase.hh"
#include "assetmod.hh"
#include "assets.hh"
#include "file.hh"

namespace hiemalia {
static auto soundEffectNames = hiemalia::makeArray<NamePair<SoundEffect>>(
    {{"mselect.wav", SoundEffect::MenuSelect},
     {"mmove.wav", SoundEffect::MenuChange},
     {"pause.wav", SoundEffect::Pause},
     {"explode.wav", SoundEffect::PlayerExplode},
     {"hit.wav", SoundEffect::PlayerHit},
     {"fire.wav", SoundEffect::PlayerFire},
     {"hiscore.wav", SoundEffect::HighScoreEntered},
     {"expsmall.wav", SoundEffect::ExplodeSmall},
     {"expmed.wav", SoundEffect::ExplodeMedium},
     {"explarge.wav", SoundEffect::ExplodeLarge},
     {"fizz.wav", SoundEffect::BulletFizz},
     {"bhit.wav", SoundEffect::BulletDamage},
     {"bnohit.wav", SoundEffect::BulletNoDamage},
     {"fire1.wav", SoundEffect::EnemyFire1},
     {"fire2.wav", SoundEffect::EnemyFire2},
     {"fire3.wav", SoundEffect::EnemyFire3},
     {"fire4.wav", SoundEffect::EnemyFire4},
     {"fire5.wav", SoundEffect::EnemyFire5},
     {"fire6.wav", SoundEffect::EnemyFire6},
     {"place.wav", SoundEffect::BlockerPlace},
     {"flip.wav", SoundEffect::DirFlip},
     {"awaywego.wav", SoundEffect::Liftoff},
     {"1up.wav", SoundEffect::ExtraLife},
     {"credit.wav", SoundEffect::Credit}});

void AudioConfig::load(ConfigSectionStore store) {
    music = store.get<bool>("Music", music);
    sound = store.get<bool>("Sound", sound);
}

void AudioConfig::save(ConfigSectionStore store) const {
    store.set<bool>("Music", music);
    store.set<bool>("Sound", sound);
}

AudioEngine::AudioEngine(std::shared_ptr<HostModule> host, GameState& state)
    : audio_(getAudioModule(host)),
      config_(state.config.section<AudioConfig>()) {}

void AudioEngine::load() {
    std::vector<sound_t> sounds(
        static_cast<std::size_t>(SoundEffect::EndOfSounds), -1);
    std::vector<std::string> tracks;
    for (const auto& pair : soundEffectNames) {
        sounds[static_cast<size_t>(pair.value)] =
            audio_->loadSound(buildAssetFilePath("sounds", pair.name));
    }

    auto file = openAssetFileRead("music", "hiemalia.sng", false);
    for (std::string line; std::getline(file, line);) {
        tracks.push_back(buildAssetFilePath("music", line));
    }

    setAssetLoadedSounds(std::move(sounds));
    setAssetLoadedMusicTracks(std::move(tracks));
}

static int getLoopCount(MusicTrack track) {
    switch (track) {
        case MusicTrack::StageStart:
        case MusicTrack::GameOver:
            return 1;
        default:
            return 0;
    }
}

static int getSoundChannel(SoundEffect sfx) {
    switch (sfx) {
        case SoundEffect::BulletFizz:
            return 0;
        case SoundEffect::PlayerFire:
            return 1;
        case SoundEffect::BulletDamage:
            return 2;
        case SoundEffect::BulletNoDamage:
            return 3;
        case SoundEffect::ExplodeSmall:
        case SoundEffect::ExplodeMedium:
        case SoundEffect::ExplodeLarge:
            return 4;
        case SoundEffect::Credit:
            return 5;
        default:
            return channelAny;
    }
}

void AudioEngine::tick() {}

void AudioEngine::gotMessage(const AudioMessage& msg) {
    switch (msg.type) {
        case AudioMessageType::PlaySound: {
            if (!config_->sound) return;
            const AudioMessageSoundEffect& e = msg.getSound();
            sound_t s = getAssets().sounds.at(static_cast<size_t>(e.sound));
            if (s != -1)
                audio_->playSound(s, e.volume, e.pan, e.pitch, 1,
                                  getSoundChannel(e.sound));
            break;
        }
        case AudioMessageType::StopSounds:
            audio_->stopSounds();
            break;
        case AudioMessageType::PlayMusic: {
            if (!config_->music) return;
            MusicTrack m = msg.getMusic();
            size_t i = static_cast<size_t>(m);
            const auto& tracks = getAssets().musicTracks;
            if (i < tracks.size())
                audio_->playMusic(tracks[i], getLoopCount(m));
            break;
        }
        case AudioMessageType::FadeOutMusic:
            audio_->fadeOutMusic();
            break;
        case AudioMessageType::StopMusic:
            audio_->stopMusic();
            break;
        case AudioMessageType::Pause:
            audio_->pause();
            break;
        case AudioMessageType::Resume:
            audio_->resume();
            break;
    }
}

bool AudioEngine::canPlayMusic() { return audio_->canPlayMusic(); }

bool AudioEngine::canPlaySound() { return audio_->canPlaySound(); }
}  // namespace hiemalia
