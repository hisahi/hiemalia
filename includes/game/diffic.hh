/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/diffic.hh: header file for game difficulty (game/diffic.cc)

#ifndef M_GAME_DIFFIC_HH
#define M_GAME_DIFFIC_HH

#include "defs.hh"
#include "gconfig.hh"

namespace hiemalia {

class GameDifficulty {
  public:
    GameDifficulty(float level);
    GameDifficulty(GameDifficultyLevel level);
    float getDifficultyLevel() const;
    float getStageSpeedMultiplier() const;
    float getFireRateMultiplier() const;
    float getBulletSpeedMultiplier() const;
    float getBulletLeadMultiplier() const;
    float getBulletSpewMultiplier() const;
    float getEnemySpeedMultiplier() const;
    float getWallSpeedMultiplier() const;

  private:
    float level_;
    float speed_;
    float lead_;
    float spew_;
    float wall_;
};

};  // namespace hiemalia

#endif  // M_GAME_DIFFIC_HH
