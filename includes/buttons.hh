/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// buttons.hh: header file for controls

#ifndef M_BUTTONS_HH
#define M_BUTTONS_HH

#include <array>
#include <stdexcept>
#include <unordered_map>

#include "array.hh"
#include "config.hh"
#include "namepair.hh"

namespace hiemalia {
enum class InputDevice { Keyboard, Gamepad };

enum class ControlInput { Up, Down, Left, Right, Fire, Pause };
enum class MenuControlInput { Up, Down, Left, Right, Select, Exit };

inline auto inputDevices = hiemalia::to_array<NamePair<InputDevice>>({
    {"Keyboard", InputDevice::Keyboard},
    {"Gamepad", InputDevice::Gamepad},
});

inline auto controlInputs = to_array<NamePair<ControlInput>>({
    {"Up", ControlInput::Up},
    {"Down", ControlInput::Down},
    {"Left", ControlInput::Left},
    {"Right", ControlInput::Right},
    {"Fire", ControlInput::Fire},
    {"Pause", ControlInput::Pause},
});

inline auto menuControlInputs = to_array<NamePair<MenuControlInput>>({
    {"Up", MenuControlInput::Up},
    {"Down", MenuControlInput::Down},
    {"Left", MenuControlInput::Left},
    {"Right", MenuControlInput::Right},
    {"Select", MenuControlInput::Select},
    {"Exit", MenuControlInput::Exit},
});

inline bool updateInputLevel(bool& state, bool flag) { return (state = flag); }

inline bool updateInputEdge(bool& state, bool flag) {
    return (state = !state && flag);
}

struct ControlState {
    bool up;
    bool down;
    bool left;
    bool right;
    bool fire;
    bool pause;

    constexpr auto operator[](ControlInput input) -> bool& {
        switch (input) {
            case ControlInput::Up:
                return up;
            case ControlInput::Down:
                return down;
            case ControlInput::Left:
                return left;
            case ControlInput::Right:
                return right;
            case ControlInput::Fire:
                return fire;
            case ControlInput::Pause:
                return pause;
        }
        throw 0;
    }

    ControlState& update(const ControlState& state) {
        updateInputLevel(up, state.up);
        updateInputLevel(down, state.down);
        updateInputLevel(left, state.left);
        updateInputLevel(right, state.right);
        updateInputLevel(fire, state.fire);
        updateInputEdge(pause, state.pause);
        return *this;
    }
};

struct MenuControlState {
    bool up;
    bool down;
    bool left;
    bool right;
    bool select;
    bool exit;

    constexpr auto operator[](MenuControlInput input) -> bool& {
        switch (input) {
            case MenuControlInput::Up:
                return up;
            case MenuControlInput::Down:
                return down;
            case MenuControlInput::Left:
                return left;
            case MenuControlInput::Right:
                return right;
            case MenuControlInput::Select:
                return select;
            case MenuControlInput::Exit:
                return exit;
        }
        throw 0;
    }

    MenuControlState& update(const MenuControlState& state) {
        updateInputEdge(up, state.up);
        updateInputEdge(down, state.down);
        updateInputEdge(left, state.left);
        updateInputEdge(right, state.right);
        updateInputEdge(select, state.select);
        updateInputEdge(exit, state.exit);
        return *this;
    }
};

using control_t = int;
constexpr static control_t controlNone = -1;

class ButtonSetup : public ConfigSection {
   public:
    ButtonSetup(std::string name, InputDevice device) noexcept
        : ConfigSection("Control" + name), name_(name), device_(device) {}

    ButtonSetup(NamePair<InputDevice> pair) noexcept
        : ConfigSection("Control" + pair.name),
          name_(pair.name),
          device_(pair.value) {}

    void load(ConfigSectionStore store) override;
    void save(ConfigSectionStore store) const override;

    InputDevice getDevice() const noexcept { return device_; }

    constexpr auto control(ControlInput input) const -> const control_t& {
        switch (input) {
            case ControlInput::Up:
                return up;
            case ControlInput::Down:
                return down;
            case ControlInput::Left:
                return left;
            case ControlInput::Right:
                return right;
            case ControlInput::Fire:
                return fire;
            case ControlInput::Pause:
                return pause;
        }
        throw 0;
    }

    constexpr auto control(ControlInput input) -> control_t& {
        return const_cast<control_t&>(control(input));
    }

    constexpr auto operator[](ControlInput input) -> control_t& {
        return control(input);
    }

    constexpr auto operator[](ControlInput input) const -> const control_t& {
        return control(input);
    }

   private:
    control_t up;
    control_t down;
    control_t left;
    control_t right;
    control_t fire;
    control_t pause;
    std::string name_;
    InputDevice device_;
};
};  // namespace hiemalia

#endif  // M_BUTTONS_HH
