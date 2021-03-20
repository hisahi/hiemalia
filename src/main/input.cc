/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// input.cc: implementation of InputEngine

#include "input.hh"

#include "buttons.hh"
#include "hbase.hh"
#include "hiemalia.hh"
#include "ibase.hh"
#include "menu.hh"

namespace hiemalia {
InputEngine::InputEngine(std::shared_ptr<HostModule> host, GameState& state)
    : input_(std::move(getInputModule(std::move(host)))) {
    for (const auto& pair : inputDevices) {
        if (input_->hasInputDevice(pair.value)) {
            auto section = state.config.section<ButtonSetup>(*input_, pair);
            input_->addInputDevice(pair.value, section).resetAllDefaults();
            state.config.sectionLoad(section);
        }
    }
}

void InputEngine::update(GameState& state) {
    input_->update(state.controls, controlsmenu_);
    if (controlsmenu_.exit) sendMessage(MenuMessage::exit());
    if (controlsmenu_.select) sendMessage(MenuMessage::select());
    if (controlsmenu_.up) sendMessage(MenuMessage::up());
    if (controlsmenu_.down) sendMessage(MenuMessage::down());
    if (controlsmenu_.left) sendMessage(MenuMessage::left());
    if (controlsmenu_.right) sendMessage(MenuMessage::right());
}

}  // namespace hiemalia
