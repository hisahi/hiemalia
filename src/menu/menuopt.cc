/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menuopt.cc: implementation of MenuOptions

#include "menu/menuopt.hh"

#include "defs.hh"
#include "menu/menuaud.hh"
#include "menu/menugame.hh"
#include "menu/menuinp.hh"
#include "menu/menuvid.hh"

namespace hiemalia {

enum Item : symbol_t {
    Item_Game,
    Item_Video,
    Item_Audio,
    Item_Input,
    Item_Back
};

void MenuOptions::begin(GameState& state) {
    option(MenuOption::button(Item_Game, "GAME"));
    option(MenuOption::button(Item_Video, "VIDEO"));
    option(MenuOption::button(Item_Audio, "AUDIO"));
    option(MenuOption::button(Item_Input, "INPUT"));
    option(MenuOption::spacer(symbol_none));
    option(MenuOption::button(Item_Back, "BACK"));
}

void MenuOptions::select(int index, symbol_t id) {
    switch (id) {
        case Item_Game:
            openMenu<MenuGameOptions>(holder_);
            break;
        case Item_Video:
            openMenu<MenuVideoOptions>(holder_);
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

MenuOptions::MenuOptions(MenuHandler& handler,
                         const std::shared_ptr<ModuleHolder>& holder)
    : Menu(handler), holder_(holder) {}

MenuOptions::~MenuOptions() noexcept {}

}  // namespace hiemalia
