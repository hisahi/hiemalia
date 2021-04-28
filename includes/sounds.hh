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
    Ambience,
    StageStart,
    Stage1,
    Stage2,
    Stage3,
    Stage4,
    Continue,
    GameOver,
    HighScore
};
enum class SoundEffect {
    MenuSelect,
    MenuChange,
    Pause,
    PlayerExplode,
    PlayerHit,
    PlayerFire,
    HighScoreEntered,
    ExplodeSmall,
    ExplodeMedium,
    ExplodeLarge,
    BulletFizz,
    BulletDamage,
    BulletNoDamage,
    EnemyFire1,
    EnemyFire2,
    EnemyFire3,
    EnemyFire4,
    EnemyFire5,
    EnemyFire6,
    BlockerPlace,
    DirFlip,
    Liftoff,
    ExtraLife,
    Credit,
    //
    EndOfSounds
};

};  // namespace hiemalia

#endif  // M_SOUNDS_HH
