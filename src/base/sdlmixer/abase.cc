/****************************************************************************/
/*                                                                          */
/*   MALPINX SOURCE CODE (C) 2020      SAMPO HIPPELAINEN (HISAHI).          */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// sdlmixer/abase.cc: implementation of base audio library on SDL2 SDL_mixer

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_mixer.h>

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include "base/sdl2.hh"
#include "defs.hh"

using std::cerr;
using std::endl;

struct SDLSoundClip {
    SDLSoundClip(Mix_Chunk *chunk,
                 std::unique_ptr<std::vector<unsigned char>> ptr)
        : sdl(chunk), data(std::move(ptr)) {}
    SDLSoundClip(SDLSoundClip &&src)
        : sdl(src.sdl), data(std::move(src.data)) {}
    SDLSoundClip(const SDLSoundClip &) = delete;
    SDLSoundClip &operator=(const SDLSoundClip &) = delete;
    Mix_Chunk *sdl;
    std::unique_ptr<std::vector<unsigned char>> data;
};

bool audio_init = false;
Mix_Music *music = nullptr;
std::vector<SDLSoundClip> sounds;
int outSampleRate;
Uint16 outFormat;
int outChannels;

void abase_init(int sampleRate) {
    if (SDL_InitSubSystem(SDL_INIT_AUDIO)) {
        cerr << "Could not initialize SDL_audio; no music/sound" << endl;
        return;
    }
    if (Mix_OpenAudio(sampleRate, MIX_DEFAULT_FORMAT, 2, 1024)) {
        cerr << "Could not initialize SDL_mixer; no music/sound" << endl;
        return;
    }
    if (!Mix_QuerySpec(&outSampleRate, &outFormat, &outChannels)) {
        Mix_CloseAudio();
        cerr << "Could not initialize SDL_mixer; no music/sound" << endl;
        return;
    }
    if (outSampleRate != S_SAMPLE_RATE_LO &&
        outSampleRate != S_SAMPLE_RATE_HI) {
        // cannot resample
        Mix_CloseAudio();
        cerr << "Could not initialize SDL_mixer; no music/sound" << endl;
        return;
    }
    Mix_AllocateChannels(16);
    Mix_ReserveChannels(3);
    audio_init = true;
}

void abase_quit() {
    if (!audio_init) return;
    Mix_CloseAudio();
    audio_init = false;
}

bool abase_ready() { return audio_init; }

void abase_music_play(const std::string &file, int loopCount) {
    if (!audio_init) return;
    if (music) Mix_FreeMusic(music);
    if (music = Mix_LoadMUS(file.c_str()))
        Mix_PlayMusic(music, loopCount || -1);
    else if (M_DEBUG)
        cerr << "could not load music " << file << ": " << Mix_GetError()
             << endl;
}

void abase_music_fadeout(int fadeMs /*= S_FADEOUT_MS */) {
    if (!audio_init) return;
    Mix_FadeOutMusic(fadeMs);
}

void abase_music_stop() {
    if (!audio_init) return;
    Mix_HaltMusic();
}

void abase_pause() {
    if (!audio_init) return;
    Mix_Pause(-1);
    Mix_PauseMusic();
}

void abase_resume() {
    if (!audio_init) return;
    Mix_ResumeMusic();
    Mix_Resume(-1);
}

std::unique_ptr<std::vector<unsigned char>> abase_convert(
    const std::vector<char> &data) {
    std::unique_ptr<std::vector<unsigned char>> result =
        std::make_unique<std::vector<unsigned char>>();
    int i;

    switch (outFormat) {
        case AUDIO_S8:
            result->reserve(data.size() * outChannels);
            for (const char &c : data)
                for (i = 0; i < outChannels; ++i)
                    result->push_back(static_cast<unsigned char>(c));
            break;
        case AUDIO_U8:
            result->reserve(data.size() * outChannels);
            for (const char &c : data)
                for (i = 0; i < outChannels; ++i)
                    result->push_back(static_cast<unsigned char>(c + 0x80));
            break;
        case AUDIO_S16LSB:
            result->reserve(data.size() * outChannels * 2);
            for (const char &c : data)
                for (i = 0; i < outChannels; ++i) {
                    result->push_back(static_cast<unsigned char>(c ^ 0x80));
                    result->push_back(static_cast<unsigned char>(c));
                }
            break;
        case AUDIO_S16MSB:
            result->reserve(data.size() * outChannels * 2);
            for (const char &c : data)
                for (i = 0; i < outChannels; ++i) {
                    result->push_back(static_cast<unsigned char>(c));
                    result->push_back(static_cast<unsigned char>(c ^ 0x80));
                }
            break;
        case AUDIO_U16LSB:
        case AUDIO_U16MSB:
            result->reserve(data.size() * outChannels * 2);
            unsigned char t;
            for (const char &c : data) {
                t = static_cast<unsigned char>(c + 0x80);
                for (i = 0; i < outChannels; ++i) {
                    result->push_back(t);
                    result->push_back(t);
                }
            }
            break;
        default:
            throw std::runtime_error("unrecognized output audio format");
    }

    return result;
}

// data = 8-bit signed mono at given samplerate
int abase_sound_load(const std::vector<char> &data, int sampleRate) {
    if (!audio_init) return -1;
    int index = sounds.size();
    auto pcm = abase_convert(data);
    Mix_Chunk *sample = Mix_QuickLoad_RAW(pcm->data(), pcm->size());
    if (!sample) {
        if (M_DEBUG)
            cerr << "could not load sound " << index << ": " << Mix_GetError()
                 << endl;
        return -1;
    }
    sounds.push_back(SDLSoundClip(sample, std::move(pcm)));
    return index;
}

int abase_sound_play(int soundNum, float volume, float pan, int playCount,
                     int channel) {
    if (!audio_init) return -1;
    if (soundNum >= sounds.size()) return -1;
    int ch = Mix_PlayChannel(channel, sounds[soundNum].sdl,
                             playCount ? playCount - 1 : -1);
    if (M_DEBUG && ch < 0)
        cerr << "Could not play sound: " << Mix_GetError() << endl;
    Mix_UnregisterAllEffects(ch);
    Uint8 left = static_cast<Uint8>(255 * volume * std::min(1.f, 1.f - pan));
    Uint8 right = static_cast<Uint8>(255 * volume * std::min(1.f, 1.f + pan));
    Mix_SetPanning(ch, left, right);
    return ch;
}

bool abase_song_playing() { return static_cast<bool>(Mix_PlayingMusic()); }

void abase_sound_fadeout(int channel, int fadeMs /*= S_FADEOUT_MS */) {
    if (!audio_init || channel < 0) return;
    Mix_FadeOutChannel(channel, fadeMs);
}

void abase_sound_stop(int channel) {
    if (!audio_init || channel < 0) return;
    Mix_HaltChannel(channel);
}

void abase_sound_fadeout_all(int fadeMs /*= S_FADEOUT_MS */) {
    if (!audio_init) return;
    Mix_FadeOutChannel(-1, fadeMs);
}

void abase_sound_stop_all() {
    if (!audio_init) return;
    Mix_HaltChannel(-1);
}
