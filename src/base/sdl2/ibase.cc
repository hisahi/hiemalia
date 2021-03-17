/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// base/sdl2/ibase.cc: implementation of InputModuleSDL2

#include <algorithm>

#include "base/sdl2.hh"
#include "base/sdl2/hbasei.hh"
#include "base/sdl2/ibasei.hh"
#include "logger.hh"

namespace hiemalia {
InputModuleSDL2::InputModuleSDL2(std::shared_ptr<HostModule> host)
    : host_(std::move(std::dynamic_pointer_cast<HostModuleSDL2>(host))),
      devices_(inputDevices.size()) {
    dynamic_assert(host_ != nullptr, "must be HostModuleSDL2!!!");
    if (SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER))
        subsystem_ = true;
    else
        logger.warn(
            "could not initialize SDL2 joystick / game controller "
            "subsystems: " +
            std::string(SDL_GetError()));
    host_->addInputModule(*this);
}

InputModuleSDL2::~InputModuleSDL2() { host_->removeInputModule(*this); }

InputModuleSDL2::InputModuleSDL2(InputModuleSDL2&& move)
    : InputModule(std::move(move)), host_(std::move(move.host_)) {
    state_ = move.state_;
    host_->addInputModule(*this);
}

InputModuleSDL2& InputModuleSDL2::operator=(InputModuleSDL2&& move) {
    InputModule::operator=(std::move(move));
    if (host_ != move.host_) {
        host_->removeInputModule(*this);
        host_ = move.host_;
        host_->addInputModule(*this);
    }
    state_ = move.state_;
    return *this;
}

void InputModuleSDL2::update(ControlState& state, MenuControlState& menustate) {
    state.update(state_);
    menustate.update(menustate_);
}

bool InputModuleSDL2::hasInputDevice(InputDevice device) const {
    switch (device) {
        case InputDevice::Keyboard:
            return true;
        case InputDevice::Gamepad:
            if (!subsystem_) return false;
            for (int i = 0; i < SDL_NumJoysticks(); ++i) {
                if (SDL_IsGameController(i)) {
                    return true;
                }
            }
            return false;
    }
    return false;
}

InputControlModule& InputModuleSDL2::addInputDevice(
    InputDevice device, ConfigSectionPtr<ButtonSetup>& config) {
    int index = static_cast<int>(device);
    dynamic_assert(devices_.at(index) == nullptr, "too many devices");
    devices_[index] = std::make_unique<InputControlModuleSDL2>(*this, config);
    std::unique_ptr<InputControlModuleSDL2>& module = devices_[index];
    switch (device) {
        case InputDevice::Keyboard:
            break;
        case InputDevice::Gamepad: {
            SDL_GameController* pad = nullptr;
            for (int i = 0; i < SDL_NumJoysticks(); ++i) {
                if (SDL_IsGameController(i)) {
                    pad = SDL_GameControllerOpen(i);
                    if (!pad) {
                        logger.warn("could not initialize game controller: " +
                                    std::string(SDL_GetError()));
                    } else
                        break;
                }
            }
            if (!pad) logger.error("no game controllers available");
            module->pad_ = pad;
            break;
        }
        default:
            never("unrecognized input device!");
    }
    return *module;
}

void InputModuleSDL2::handle(const SDL_Event& event) {
    InputDevice device;
    switch (event.type) {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            device = InputDevice::Keyboard;
            break;
        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP:
            device = InputDevice::Gamepad;
            break;
    }
    std::unique_ptr<InputControlModuleSDL2>& module =
        devices_.at(static_cast<int>(device));
    if (module) module->handle(state_, menustate_, event);
}

InputControlModuleSDL2::InputControlModuleSDL2(
    InputModuleSDL2& host, const ConfigSectionPtr<ButtonSetup>& config)
    : InputControlModule(host, config), host_(host) {}

InputControlModuleSDL2::InputControlModuleSDL2(InputControlModuleSDL2&& move)
    : InputControlModule(move.host_,
                         std::dynamic_pointer_cast<ButtonSetup>(move.config_)),
      host_(move.host_),
      setting_key_(move.setting_key_),
      set_next_(move.set_next_) {
    std::swap(joy_, move.joy_);
    std::swap(pad_, move.pad_);
}

InputControlModuleSDL2& InputControlModuleSDL2::operator=(
    InputControlModuleSDL2&& move) {
    std::swap(joy_, move.joy_);
    std::swap(pad_, move.pad_);
    setting_key_ = move.setting_key_;
    set_next_ = move.set_next_;
    return *this;
}

InputControlModuleSDL2::~InputControlModuleSDL2() {
    if (joy_) SDL_JoystickClose(joy_);
    if (pad_) SDL_GameControllerClose(pad_);
}

static void updateMenuStateKeyboard(MenuControlState& state, bool down,
                                    control_t button) {
    SDL_Scancode code = static_cast<SDL_Scancode>(button);
    switch (code) {
        case SDL_SCANCODE_UP:
            state.up = down;
            break;
        case SDL_SCANCODE_DOWN:
            state.down = down;
            break;
        case SDL_SCANCODE_LEFT:
            state.left = down;
            break;
        case SDL_SCANCODE_RIGHT:
            state.right = down;
            break;
        case SDL_SCANCODE_SPACE:
        case SDL_SCANCODE_RETURN:
            state.select = down;
            break;
        case SDL_SCANCODE_ESCAPE:
            state.exit = down;
            break;
        default:
            break;
    }
}

static void updateMenuStateGamepad(MenuControlState& state, bool down,
                                   control_t button) {
    SDL_GameControllerButton code =
        static_cast<SDL_GameControllerButton>(button);
    switch (code) {
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            state.up = down;
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            state.down = down;
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            state.left = down;
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            state.right = down;
            break;
        case SDL_CONTROLLER_BUTTON_A:
            state.select = down;
            break;
        case SDL_CONTROLLER_BUTTON_B:
            state.exit = down;
            break;
        default:
            break;
    }
}

static SDL_Scancode keyboardDefault(ControlInput input) {
    switch (input) {
        case ControlInput::Up:
            return SDL_SCANCODE_UP;
        case ControlInput::Down:
            return SDL_SCANCODE_DOWN;
        case ControlInput::Left:
            return SDL_SCANCODE_LEFT;
        case ControlInput::Right:
            return SDL_SCANCODE_RIGHT;
        case ControlInput::Fire:
            return SDL_SCANCODE_Z;
        case ControlInput::Pause:
            return SDL_SCANCODE_P;
        default:
            break;
    }
    never("unrecognized control");
}

static SDL_GameControllerButton gamepadDefault(ControlInput input) {
    switch (input) {
        case ControlInput::Up:
            return SDL_CONTROLLER_BUTTON_DPAD_UP;
        case ControlInput::Down:
            return SDL_CONTROLLER_BUTTON_DPAD_DOWN;
        case ControlInput::Left:
            return SDL_CONTROLLER_BUTTON_DPAD_LEFT;
        case ControlInput::Right:
            return SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
        case ControlInput::Fire:
            return SDL_CONTROLLER_BUTTON_A;
        case ControlInput::Pause:
            return SDL_CONTROLLER_BUTTON_START;
        default:
            break;
    }
    never("unrecognized control");
}

void InputControlModuleSDL2::handle(ControlState& state,
                                    MenuControlState& menustate,
                                    const SDL_Event& event) {
    ButtonSetup& setup = dynamic_cast<ButtonSetup&>(*config_);
    bool down;
    control_t button;
    switch (event.type) {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            if (device_ != InputDevice::Keyboard) return;
            down = event.type == SDL_KEYDOWN;
            button = static_cast<control_t>(event.key.keysym.scancode);
            updateMenuStateKeyboard(menustate, down, button);
            break;
        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP:
            if (device_ != InputDevice::Gamepad || !pad_) return;
            down = event.type == SDL_CONTROLLERBUTTONDOWN;
            button = static_cast<control_t>(event.cbutton.button);
            updateMenuStateGamepad(menustate, down, button);
            break;
    }

    if (down && setting_key_) {
        setup[set_next_] = button;
        setting_key_ = false;
        return;
    }

    for (auto& pair : controlInputs) {
        ControlInput input = pair.value;
        if (setup[input] == button) {
            state[input] = down;
        }
    }
}

std::string InputControlModuleSDL2::getButtonName(ControlInput input) const {
    ButtonSetup& setup = dynamic_cast<ButtonSetup&>(*config_);
    control_t button = setup[input];
    switch (device_) {
        case InputDevice::Keyboard:
            return std::string(
                SDL_GetScancodeName(static_cast<SDL_Scancode>(button)));
        case InputDevice::Gamepad:
            return std::string(SDL_GameControllerGetStringForButton(
                static_cast<SDL_GameControllerButton>(button)));
        default:
            never("unrecognized input device!");
    }
}

void InputControlModuleSDL2::setNextButton(ControlInput input) {
    setting_key_ = true;
    set_next_ = input;
}

void InputControlModuleSDL2::resetDefault(ControlInput input) {
    ButtonSetup& setup = dynamic_cast<ButtonSetup&>(*config_);
    switch (device_) {
        case InputDevice::Keyboard:
            setup[input] = static_cast<control_t>(keyboardDefault(input));
            break;
        case InputDevice::Gamepad:
            setup[input] = static_cast<control_t>(gamepadDefault(input));
            break;
        default:
            never("unrecognized input device!");
    }
}

void InputControlModuleSDL2::resetAllDefaults() {
    for (auto& pair : controlInputs) {
        resetDefault(pair.value);
    }
}
}  // namespace hiemalia
