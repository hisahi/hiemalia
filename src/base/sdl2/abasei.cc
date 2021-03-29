/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// base/sdl2/abasei.cc: SDL(_mixer) 2 audio module impl

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include "base/sdl2.hh"
#include "base/sdl2/abasei.hh"
#include "base/sdl2/hbasei.hh"
#include "defs.hh"
#include "logger.hh"

namespace hiemalia {
SDLMixer::SDLMixer() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024))
        throw SDLMixerException("could not initialize SDL_mixer 2");
    else
        own_ = true;
}

SDLMixer::~SDLMixer() noexcept {
    if (own_) Mix_CloseAudio();
}

SDLMixer::SDLMixer(SDLMixer &&move) noexcept : own_(move.own_) {}

SDLMixer &SDLMixer::operator=(SDLMixer &&move) noexcept {
    if (own_ && move.own_) Mix_CloseAudio();
    std::swap(own_, move.own_);
    return *this;
}

AudioModuleSDLMixer2::AudioModuleSDLMixer2(std::shared_ptr<HostModule> host)
    : host_(std::move(std::dynamic_pointer_cast<HostModuleSDL2>(host))) {
    dynamic_assert(host_ != nullptr, "must be HostModuleSDL2!!!");
    if (SDL_InitSubSystem(SDL_INIT_AUDIO))
        throw SDLException("could not initialize SDL2 audio subsystem");
    mixer_ = std::make_unique<SDLMixer>();
    Mix_AllocateChannels(16);
    Mix_ReserveChannels(4);
}

AudioModuleSDLMixer2::AudioModuleSDLMixer2(AudioModuleSDLMixer2 &&move) noexcept
    : AudioModule(std::move(move)),
      host_(std::move(move.host_)),
      mixer_(std::move(move.mixer_)) {}

AudioModuleSDLMixer2 &AudioModuleSDLMixer2::operator=(
    AudioModuleSDLMixer2 &&move) noexcept {
    AudioModule::operator=(std::move(move));
    host_ = std::move(move.host_);
    mixer_ = std::move(move.mixer_);
    return *this;
}

AudioModuleSDLMixer2::~AudioModuleSDLMixer2() noexcept {
    if (music_) Mix_FreeMusic(music_);
}

void AudioModuleSDLMixer2::tick() {}

void AudioModuleSDLMixer2::pause() {
    Mix_Pause(-1);
    Mix_PauseMusic();
}

void AudioModuleSDLMixer2::resume() {
    Mix_ResumeMusic();
    Mix_Resume(-1);
}

void AudioModuleSDLMixer2::playMusic(const std::string &filename,
                                     size_t loopCount) {
    if (music_) Mix_FreeMusic(music_);
    music_ = Mix_LoadMUS(filename.c_str());
    if (music_) {
        Mix_PlayMusic(music_, loopCount > 0 ? loopCount - 1 : -1);
    } else {
        LOG_WARN("Failed to load music file %s: %s", filename, Mix_GetError());
    }
}

void AudioModuleSDLMixer2::stopMusic() { Mix_HaltMusic(); }

bool AudioModuleSDLMixer2::isMusicPlaying() {
    return static_cast<bool>(Mix_PlayingMusic());
}

sound_t AudioModuleSDLMixer2::loadSound(const std::string &filename) {
    sound_t index = sounds_.size();
    Mix_Chunk *sample = Mix_LoadWAV(filename.c_str());
    if (!sample) {
        LOG_WARN("Failed to load sound file %s: %s", filename, Mix_GetError());
        return -1;
    }
    sounds_.push_back(SDLSoundClip(sample));
    return index;
}

void AudioModuleSDLMixer2::playSound(sound_t soundId, float volume, float pan,
                                     size_t loopCount, int channel) {
    if (soundId < 0 || soundId >= static_cast<int>(sounds_.size())) {
        return;
    }
    int ch = Mix_PlayChannel(channel, sounds_[soundId].sdl,
                             loopCount ? loopCount - 1 : -1);
    if (ch < 0) {
        LOG_WARN("Failed to play sound effect %d: %s", soundId, Mix_GetError());
        return;
    }
    Mix_UnregisterAllEffects(ch);
    Uint8 left = static_cast<Uint8>(255 * volume * std::min(1.f, 1.f - pan));
    Uint8 right = static_cast<Uint8>(255 * volume * std::min(1.f, 1.f + pan));
    Mix_SetPanning(ch, left, right);
}

void AudioModuleSDLMixer2::stopSound(int channel) { Mix_HaltChannel(channel); }

void AudioModuleSDLMixer2::stopSounds() { Mix_HaltChannel(-1); }

}  // namespace hiemalia
