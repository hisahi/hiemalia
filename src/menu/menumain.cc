/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menumain.cc: implementation of MenuMain

#include "menumain.hh"

#include "defs.hh"
#include "hiemalia.hh"
#include "logic.hh"
#include "menuopt.hh"

namespace hiemalia {

enum Item : symbol_t {
    Item_Game,
    Item_Options,
    Item_Scores,
    Item_Help,
    Item_Exit
};

void MenuMain::begin(GameState& state) {
    option(MenuOption::make_button(Item_Game, "GAME"));
    option(MenuOption::make_button(Item_Options, "OPTIONS"));
    option(MenuOption::make_button(Item_Scores, "HIGH SCORES"));
    option(MenuOption::make_button(Item_Help, "HELP"));
    option(MenuOption::make_button(Item_Exit, "EXIT"));
}

void MenuMain::select(int index, symbol_t id) {
    switch (id) {
        case Item_Game:
            sendMessage(LogicMessage::startGame());
            closeMenu();
            break;
        case Item_Options:
            openMenu<MenuOptions>(holder_);
            break;
        case Item_Scores:
            break;
        case Item_Help:
            break;
        case Item_Exit:
            sendMessage(GameMessage::quit());
            break;
    }
}

void MenuMain::end(GameState& state) {}

MenuMain::MenuMain(MenuMain&& move) noexcept
    : Menu(std::move(move)), holder_(std::move(move.holder_)) {}

MenuMain& MenuMain::operator=(MenuMain&& move) noexcept {
    Menu::operator=(std::move(move));
    holder_ = std::move(holder_);
    return *this;
}

MenuMain::MenuMain(MenuHandler& handler,
                   const std::shared_ptr<ModuleHolder>& holder)
    : Menu(handler), holder_(holder) {
    makeUncloseable();
}

MenuMain::~MenuMain() {}

}  // namespace hiemalia
