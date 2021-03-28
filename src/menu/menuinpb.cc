/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menuinpb.cc: implementation of input control menu

#include "menuinpb.hh"

#include "controls.hh"
#include "defs.hh"
#include "input.hh"
#include "logger.hh"

namespace hiemalia {

enum Item : symbol_t { Item_Back = index_to_symbol(9999) };

std::string MenuInputControls::title() const noexcept { return title_; }

void MenuInputControls::begin(GameState& state) {
    for (const auto& pair : controlInputs) {
        ControlInput ctrl = pair.value;
        option(MenuOption::input(index_to_symbol(static_cast<int>(ctrl)),
                                 pair.name, getControlText(ctrl)));
    }
    option(MenuOption::spacer(symbol_none));
    option(MenuOption::button(Item_Back, "BACK"));
}

void MenuInputControls::select(int index, symbol_t id) {
    if (id == Item_Back) {
        closeMenu();
        return;
    }
    ControlInput ctrl = static_cast<ControlInput>(symbol_to_index(id));
    for (int i = 0, e = options_.size(); i < e; ++i) {
        auto& option = options_[i];
        option.enabled = option.id == id;
        if (option.enabled) {
            option.asInput().value = "?" + option.asInput().value + "?";
            settingButton_ = i;
        }
        option.dirty = true;
    }
    module_.get().setNextButton(ctrl);
}

void MenuInputControls::end(GameState& state) {
    state.config.sectionSave(module_.get().getConfig());
}

void MenuInputControls::gotMessage(const InputMenuMessage& msg) {
    switch (msg.type) {
        case InputMenuMessageType::NewButtonSet:
            options_[settingButton_].asInput().value =
                getControlText(msg.button());
            for (auto& option : options_)
                option.dirty = option.enabled =
                    MenuOption::defaultEnabled(option.type);
            settingButton_ = -1;
            break;
    }
}

std::string MenuInputControls::getControlText(ControlInput input) {
    return module_.get().getButtonName(input);
}

MenuInputControls::MenuInputControls(MenuInputControls&& move) noexcept
    : Menu(std::move(move)),
      holder_(std::move(move.holder_)),
      title_(move.title_),
      device_(move.device_),
      module_(std::move(move.module_)),
      settingButton_(move.settingButton_) {}

MenuInputControls& MenuInputControls::operator=(
    MenuInputControls&& move) noexcept {
    Menu::operator=(std::move(move));
    holder_ = std::move(holder_);
    title_ = move.title_;
    device_ = move.device_;
    module_ = std::move(module_);
    settingButton_ = move.settingButton_;
    return *this;
}

MenuInputControls::MenuInputControls(
    MenuHandler& handler, const std::shared_ptr<ModuleHolder>& holder,
    NamePair<InputDevice> pair, InputControlModule& module)
    : Menu(handler),
      holder_(holder),
      title_(pair.name),
      device_(pair.value),
      module_(module) {}

MenuInputControls::~MenuInputControls() {}

}  // namespace hiemalia