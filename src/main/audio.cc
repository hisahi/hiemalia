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
void AudioConfig::load(ConfigSectionStore store) {
    music = store.get<bool>("Music", music);
    sound = store.get<bool>("Sound", sound);
}

void AudioConfig::save(ConfigSectionStore store) const {
    store.set<bool>("Music", music);
    store.set<bool>("Sound", sound);
}

AudioEngine::AudioEngine(std::shared_ptr<HostModule> host, GameState& state)
    : audio_(std::move(getAudioModule(std::move(host)))),
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
        default:
            return 0;
    }
}

void AudioEngine::tick() {}

void AudioEngine::gotMessage(const AudioMessage& msg) {
    switch (msg.type) {
        case AudioMessageType::PlaySound: {
            if (!config_->sound) return;
            const AudioMessageSoundEffect& e = msg.getSound();
            sound_t s = getAssets().sounds.at(static_cast<size_t>(e.sound));
            if (s != -1) audio_->playSound(s, e.volume, e.pan, 1, channelAny);
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
}  // namespace hiemalia
