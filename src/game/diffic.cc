/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/diffic.cc: implementation of GameDifficulty

#include "game/diffic.hh"

namespace hiemalia {
GameDifficulty::GameDifficulty(float level) : level_(level) {}

float GameDifficulty::getFireRateMultiplier() const { return level_; }

float GameDifficulty::getBulletSpeedMultiplier() const { return level_; }

float GameDifficulty::getBulletSpewMultiplier() const { return level_; }

}  // namespace hiemalia
