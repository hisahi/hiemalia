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
InputEngine::InputEngine(const std::shared_ptr<HostModule>& host,
                         GameState& state)
    : input_(getInputModule(host)) {
    for (const auto& pair : inputDevices) {
        if (input_->hasInputDevice(pair.value)) {
            auto section = state.config.section<ButtonSetup>(*input_, pair);
            input_->addInputDevice(pair.value, section).resetAllDefaults();
            state.config.sectionLoad(section);
        }
    }
}

bool InputEngine::hasInputDevice(InputDevice device) const {
    return input_->hasInputDevice(device);
}

InputControlModule& InputEngine::getInputDevice(InputDevice device) {
    return input_->getInputDevice(device);
}

void InputEngine::update(GameState& state, float interval) {
    input_->update(state.controls, controlsmenu_, interval);
    if (controlsmenu_.exit) sendMessage(MenuMessage::exit());
    if (controlsmenu_.select) sendMessage(MenuMessage::select());
    if (controlsmenu_.up) sendMessage(MenuMessage::up());
    if (controlsmenu_.down) sendMessage(MenuMessage::down());
    if (controlsmenu_.left) sendMessage(MenuMessage::left());
    if (controlsmenu_.right) sendMessage(MenuMessage::right());
}

bool RepeatControl::update(bool down, float interval) {
    if (!down || !lastDown_) {
        lastDown_ = down;
        if (down) {
            repeatCount_ = 0;
            repeatTime_ = 0;
        }
        return down;
    }
    // down && lastDown_
    float maxTime;
    if (repeatCount_ == 0) {
        maxTime = 0.4f;
    } else if (repeatCount_ < 4) {
        maxTime = 1.0f / 6;
    } else {
        maxTime = 1.0f / 12;
    }
    bool d = false;
    repeatTime_ += interval;
    if (repeatTime_ > maxTime) {
        d = true;
        repeatCount_ = std::min(100, repeatCount_ + 1);
        repeatTime_ -= maxTime;
    }
    return d;
}

}  // namespace hiemalia
