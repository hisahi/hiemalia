/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// audio.cc: implementation of AudioEngine

#include "audio.hh"

#include "abase.hh"

namespace hiemalia {
AudioEngine::AudioEngine(std::shared_ptr<HostModule> host)
    : audio_(std::move(getAudioModule(std::move(host)))) {}

void AudioEngine::load() {}

void AudioEngine::tick() {}

void AudioEngine::gotMessage(const AudioMessage& msg) {}
}  // namespace hiemalia
