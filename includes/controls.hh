/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// controls.hh: header file for controls

#ifndef M_CONTROLS_HH
#define M_CONTROLS_HH

#include <array>

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

inline bool updateInputLevel(bool& next, bool& prev, bool state) {
    next = state;
    prev = state;
    return next;
}

inline bool updateInputEdge(bool& next, bool& prev, bool state) {
    next = state && !prev;
    prev = state;
    return next;
}

struct ControlState {
    bool up{false};
    bool down{false};
    bool left{false};
    bool right{false};
    bool fire{false};
    bool pause{false};

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
        updateInputLevel(up, up_prev_, state.up);
        updateInputLevel(down, down_prev_, state.down);
        updateInputLevel(left, left_prev_, state.left);
        updateInputLevel(right, right_prev_, state.right);
        updateInputLevel(fire, fire_prev_, state.fire);
        updateInputEdge(pause, pause_prev_, state.pause);
        return *this;
    }

   private:
    bool up_prev_{false};
    bool down_prev_{false};
    bool left_prev_{false};
    bool right_prev_{false};
    bool fire_prev_{false};
    bool pause_prev_{false};
};

struct MenuControlState {
    bool exit{false};
    bool select{false};
    bool up{false};
    bool down{false};
    bool left{false};
    bool right{false};

    constexpr auto operator[](MenuControlInput input) -> bool& {
        switch (input) {
            case MenuControlInput::Exit:
                return exit;
            case MenuControlInput::Select:
                return select;
            case MenuControlInput::Up:
                return up;
            case MenuControlInput::Down:
                return down;
            case MenuControlInput::Left:
                return left;
            case MenuControlInput::Right:
                return right;
        }
        throw 0;
    }

    MenuControlState& update(const MenuControlState& state) {
        updateInputEdge(exit, exit_prev_, state.exit);
        updateInputEdge(select, select_prev_, state.select);
        updateInputEdge(up, up_prev_, state.up);
        updateInputEdge(down, down_prev_, state.down);
        updateInputEdge(left, left_prev_, state.left);
        updateInputEdge(right, right_prev_, state.right);
        return *this;
    }

   private:
    bool exit_prev_{false};
    bool select_prev_{false};
    bool up_prev_{false};
    bool down_prev_{false};
    bool left_prev_{false};
    bool right_prev_{false};
};

using control_t = int;
constexpr static control_t controlNone = -1;

};  // namespace hiemalia

#endif  // M_CONTROLS_HH
