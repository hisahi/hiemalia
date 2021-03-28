/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// sounds.hh: header file for the sound and music list

#ifndef M_SOUNDS_HH
#define M_SOUNDS_HH

#include <array>

#include "array.hh"
#include "namepair.hh"

namespace hiemalia {
enum class MusicTrack {
    StageStart,
    Stage1,
    Stage2,
    Stage3,
    GameOver,
    HighScore
};
enum class SoundEffect { MenuSelect, MenuChange, Pause, EndOfSounds };

inline auto soundEffectNames = hiemalia::to_array<NamePair<SoundEffect>>({
    {"mselect.wav", SoundEffect::MenuSelect},
    {"mmove.wav", SoundEffect::MenuChange},
    {"pause.wav", SoundEffect::Pause},
});

};  // namespace hiemalia

#endif  // M_SOUNDS_HH