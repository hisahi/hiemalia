/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menupaus.cc: implementation of MenuPause

#include "menupaus.hh"

#include "defs.hh"
#include "game/gamemsg.hh"
#include "hiemalia.hh"

namespace hiemalia {

enum Item : symbol_t { Item_Continue, Item_Exit };

void MenuPause::begin(GameState& state) {
    option(MenuOption::button(Item_Continue, "CONTINUE"));
    option(MenuOption::button(Item_Exit, "EXIT"));
}

void MenuPause::select(int index, symbol_t id) {
    switch (id) {
        case Item_Continue:
            unpause_ = true;
            closeMenu();
            break;
        case Item_Exit:
            unpause_ = false;
            closeMenu();
            break;
    }
}

void MenuPause::end(GameState& state) {
    if (unpause_) {
        sendMessage(GameMessage::continueGame());
    } else {
        sendMessage(GameMessage::exitGame());
    }
}

MenuPause::MenuPause(MenuPause&& move) noexcept : Menu(std::move(move)) {}

MenuPause& MenuPause::operator=(MenuPause&& move) noexcept {
    Menu::operator=(std::move(move));
    return *this;
}

MenuPause::MenuPause(MenuHandler& handler) : Menu(handler) {}

MenuPause::~MenuPause() {}

}  // namespace hiemalia
