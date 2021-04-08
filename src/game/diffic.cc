/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/diffic.cc: implementation of GameDifficulty

#include "game/diffic.hh"

#include "math.hh"

namespace hiemalia {
GameDifficulty::GameDifficulty(float level) : level_(level) {}

float GameDifficulty::getDifficultyLevel() const { return level_; }

float GameDifficulty::getStageSpeedMultiplier() const { return sqrt(level_); }

float GameDifficulty::getFireRateMultiplier() const { return level_; }

float GameDifficulty::getBulletSpeedMultiplier() const { return level_; }

float GameDifficulty::getBulletLeadMultiplier() const { return level_; }

float GameDifficulty::getBulletSpewMultiplier() const { return 1.0f / level_; }

float GameDifficulty::getEnemySpeedMultiplier() const { return level_; }

}  // namespace hiemalia
