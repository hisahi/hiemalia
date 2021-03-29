/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menuinp.cc: implementation of input device menu

#include "menuinp.hh"

#include "controls.hh"
#include "defs.hh"
#include "input.hh"
#include "logger.hh"
#include "menuinpb.hh"

namespace hiemalia {

enum Item : symbol_t { Item_Back = index_to_symbol(9999) };

void MenuInputDevices::begin(GameState& state) {
    InputEngine& inp = *holder_->input;
    for (int i = 0, e = inputDevices.size(); i < e; ++i) {
        const auto& pair = inputDevices[i];
        InputDevice device = pair.value;
        option(MenuOption::button(index_to_symbol(i), pair.name,
                                  inp.hasInputDevice(device)));
    }
    option(MenuOption::spacer(symbol_none));
    option(MenuOption::button(Item_Back, "BACK"));
}

void MenuInputDevices::select(int index, symbol_t id) {
    if (id == Item_Back) {
        closeMenu();
        return;
    }
    int i = symbol_to_index(id);
    InputEngine& inp = *holder_->input;
    openMenu<MenuInputControls>(holder_, inputDevices[i],
                                inp.getInputDevice(inputDevices[i].value));
}

void MenuInputDevices::end(GameState& state) {}

MenuInputDevices::MenuInputDevices(MenuInputDevices&& move) noexcept
    : Menu(std::move(move)), holder_(std::move(move.holder_)) {}

MenuInputDevices& MenuInputDevices::operator=(
    MenuInputDevices&& move) noexcept {
    Menu::operator=(std::move(move));
    holder_ = std::move(move.holder_);
    return *this;
}

MenuInputDevices::MenuInputDevices(MenuHandler& handler,
                                   const std::shared_ptr<ModuleHolder>& holder)
    : Menu(handler), holder_(holder) {}

MenuInputDevices::~MenuInputDevices() noexcept {}

}  // namespace hiemalia
