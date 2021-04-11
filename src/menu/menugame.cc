/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menugame.cc: implementation of game option menu

#include "menu/menugame.hh"

#include "audio.hh"
#include "controls.hh"
#include "defs.hh"
#include "hiemalia.hh"
#include "input.hh"
#include "logger.hh"

namespace hiemalia {

enum Item : symbol_t { Item_Difficulty, Item_Back };

void MenuGameOptions::begin(GameState& state) {
    state.config.sectionLoad(holder_->gconfig);
    GameConfig& config = *holder_->gconfig;
    option(MenuOption::select(Item_Difficulty, "DIFFICULTY",
                              {"EASY", "NORMAL", "HARD"},
                              static_cast<int>(config.difficulty)));
    option(MenuOption::spacer(symbol_none));
    option(MenuOption::button(Item_Back, "BACK"));
}

void MenuGameOptions::select(int index, symbol_t id) {
    GameConfig& config = *holder_->gconfig;
    switch (id) {
        case Item_Difficulty:
            config.difficulty = static_cast<GameDifficultyLevel>(
                options_[index].asSelect().index);
            break;
        case Item_Back:
            closeMenu();
            break;
    }
}

void MenuGameOptions::end(GameState& state) {
    state.config.sectionSave(holder_->gconfig);
}

MenuGameOptions::MenuGameOptions(MenuHandler& handler,
                                 const std::shared_ptr<ModuleHolder>& holder)
    : Menu(handler), holder_(holder) {}

MenuGameOptions::~MenuGameOptions() noexcept {}

}  // namespace hiemalia
