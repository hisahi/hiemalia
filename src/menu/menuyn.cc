/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menuyn.cc: implementation of MenuYesNo

#include "menu/menuyn.hh"

#include <utility>

#include "defs.hh"
#include "game/gamemsg.hh"
#include "hiemalia.hh"

namespace hiemalia {

enum Item : symbol_t { Item_Yes, Item_No };

void MenuYesNo::begin(GameState& state) {
    option(MenuOption::button(Item_Yes, "YES"));
    option(MenuOption::button(Item_No, "NO"));
    defaultItem = Item_No;
}

void MenuYesNo::select(int index, symbol_t id) {
    switch (id) {
        case Item_Yes:
            yes_ = true;
            closeMenu();
            break;
        case Item_No:
            no_ = true;
            closeMenu();
            break;
    }
}

void MenuYesNo::end(GameState& state) {
    if (no_)
        sendMessage(MenuMessage::no(myId_));
    else if (yes_)
        sendMessage(MenuMessage::yes(myId_));
}

MenuYesNo::MenuYesNo(MenuHandler& handler, symbol_t id, std::string title)
    : Menu(handler), myId_(id), title_(std::move(title)) {}

}  // namespace hiemalia
