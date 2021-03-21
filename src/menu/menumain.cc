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

namespace hiemalia {

enum Item : symbol_t { Item_Game, Item_Options, Item_Help, Item_Exit };

void MenuMain::begin(GameState& state) {
    option(MenuOption::make_button(Item_Game, "GAME"));
    option(MenuOption::make_button(Item_Options, "OPTIONS"));
    option(MenuOption::make_button(Item_Help, "HELP"));
    option(MenuOption::make_button(Item_Exit, "EXIT"));
}

void MenuMain::select(int index, symbol_t id) {
    switch (id) {
        case Item_Game:
            break;
        case Item_Options:
            break;
        case Item_Help:
            break;
        case Item_Exit:
            sendMessage(GameMessage::quit());
            break;
    }
}

void MenuMain::end(GameState& state) {}

MenuMain::MenuMain(MenuMain&& move) noexcept : Menu(std::move(move)) {}

MenuMain& MenuMain::operator=(MenuMain&& move) noexcept {
    Menu::operator=(std::move(move));
    return *this;
}

MenuMain::MenuMain() : Menu(nullptr) {}

MenuMain::~MenuMain() {}

}  // namespace hiemalia
