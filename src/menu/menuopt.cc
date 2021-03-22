/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menuopt.cc: implementation of MenuOptions

#include "menuopt.hh"

#include "defs.hh"
#include "menuaud.hh"
#include "menuinp.hh"

namespace hiemalia {

enum Item : symbol_t { Item_Graphics, Item_Audio, Item_Input, Item_Back };

void MenuOptions::begin(GameState& state) {
    option(MenuOption::button(Item_Graphics, "GRAPHICS", false));
    option(MenuOption::button(Item_Audio, "AUDIO"));
    option(MenuOption::button(Item_Input, "INPUT"));
    option(MenuOption::spacer(symbol_none));
    option(MenuOption::button(Item_Back, "BACK"));
}

void MenuOptions::select(int index, symbol_t id) {
    switch (id) {
        case Item_Graphics:
            break;
        case Item_Audio:
            openMenu<MenuAudioOptions>(holder_);
            break;
        case Item_Input:
            openMenu<MenuInputDevices>(holder_);
            break;
        case Item_Back:
            closeMenu();
            break;
    }
}

void MenuOptions::end(GameState& state) {}

MenuOptions::MenuOptions(MenuOptions&& move) noexcept
    : Menu(std::move(move)), holder_(std::move(move.holder_)) {}

MenuOptions& MenuOptions::operator=(MenuOptions&& move) noexcept {
    Menu::operator=(std::move(move));
    holder_ = std::move(holder_);
    return *this;
}

MenuOptions::MenuOptions(MenuHandler& handler,
                         const std::shared_ptr<ModuleHolder>& holder)
    : Menu(handler), holder_(holder) {}

MenuOptions::~MenuOptions() {}

}  // namespace hiemalia
