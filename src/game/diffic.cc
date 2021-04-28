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
static float getDifficultyLevelFloat(GameDifficultyLevel level) {
    switch (level) {
        case GameDifficultyLevel::Easy:
            return 0.75f;
        case GameDifficultyLevel::Normal:
            return 1.0f;
        case GameDifficultyLevel::Hard:
            return 1.5f;
        default:
            never("invalid difficulty level");
    }
}

GameDifficulty::GameDifficulty(float level)
    : level_(level),
      speed_(pow(level, 0.75f)),
      lead_(std::min(1.0f, sqrt(level_ * 0.75f))),
      spew_(1.0f / level),
      wall_(std::min(1.0f, speed_)) {}

GameDifficulty::GameDifficulty(GameDifficultyLevel level)
    : GameDifficulty(getDifficultyLevelFloat(level)) {}

float GameDifficulty::getDifficultyLevel() const { return level_; }

float GameDifficulty::getStageSpeedMultiplier() const { return speed_; }

float GameDifficulty::getFireRateMultiplier() const { return level_; }

float GameDifficulty::getBulletSpeedMultiplier() const { return level_; }

float GameDifficulty::getBulletLeadMultiplier() const { return lead_; }

float GameDifficulty::getBulletSpewMultiplier() const { return spew_; }

float GameDifficulty::getEnemySpeedMultiplier() const { return level_; }

float GameDifficulty::getWallSpeedMultiplier() const { return wall_; }

}  // namespace hiemalia
