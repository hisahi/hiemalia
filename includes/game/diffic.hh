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

namespace hiemalia {

class GameDifficulty {
  public:
    GameDifficulty(float level);
    float getDifficultyLevel() const;
    float getStageSpeedMultiplier() const;
    float getFireRateMultiplier() const;
    float getBulletSpeedMultiplier() const;
    float getBulletLeadMultiplier() const;
    float getBulletSpewMultiplier() const;
    float getEnemySpeedMultiplier() const;

  private:
    float level_;
};

};  // namespace hiemalia

#endif  // M_GAME_DIFFIC_HH
