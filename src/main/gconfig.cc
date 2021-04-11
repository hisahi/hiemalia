/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// gconfig.cc: implementation of GameConfig

#include "gconfig.hh"

namespace hiemalia {
void GameConfig::load(ConfigSectionStore store) {
    difficulty = static_cast<GameDifficultyLevel>(
        store.get<int>("Difficulty", static_cast<int>(difficulty)));
    switch (difficulty) {
        case GameDifficultyLevel::Easy:
        case GameDifficultyLevel::Normal:
        case GameDifficultyLevel::Hard:
            break;
        default:
            difficulty = GameDifficultyLevel::Normal;
    }
}

void GameConfig::save(ConfigSectionStore store) const {
    store.set<int>("Difficulty", static_cast<int>(difficulty));
}
}  // namespace hiemalia
