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
    maxContinues = store.get<int>("MaxContinues", maxContinues);
    arcadeFreePlay = store.get<bool>("ArcadeFreePlay", arcadeFreePlay);
    arcadeMaxCredits = store.get<int>("ArcadeMaxCredits", arcadeMaxCredits);
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
    store.set<int>("MaxContinues", maxContinues);
    store.set<bool>("ArcadeFreePlay", arcadeFreePlay);
    store.set<int>("ArcadeMaxCredits", arcadeMaxCredits);
}
}  // namespace hiemalia
