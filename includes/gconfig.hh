/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// gconfig.hh: header file for the game config

#ifndef M_GCONFIG_HH
#define M_GCONFIG_HH

#include "config.hh"

namespace hiemalia {
enum class GameDifficultyLevel {
    Easy,
    Normal,
    Hard,
};

class GameConfig : public ConfigSection {
  public:
    GameConfig() noexcept : ConfigSection("Game") {}

    void load(ConfigSectionStore store) override;
    void save(ConfigSectionStore store) const override;

    GameDifficultyLevel difficulty{GameDifficultyLevel::Normal};
};

}  // namespace hiemalia

#endif  // M_GCONFIG_HH
