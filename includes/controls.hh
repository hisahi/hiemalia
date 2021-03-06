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

enum class ControlInput { Up, Down, Left, Right, Forward, Back, Fire, Pause };
enum class MenuControlInput { Up, Down, Left, Right, Select, Exit };

inline auto inputDevices = hiemalia::makeArray<NamePair<InputDevice>>({
    {"Keyboard", InputDevice::Keyboard},
    {"Gamepad", InputDevice::Gamepad},
});

inline auto controlInputs = makeArray<NamePair<ControlInput>>({
    {"Up", ControlInput::Up},
    {"Down", ControlInput::Down},
    {"Left", ControlInput::Left},
    {"Right", ControlInput::Right},
    {"Forward", ControlInput::Forward},
    {"Back", ControlInput::Back},
    {"Fire", ControlInput::Fire},
    {"Pause", ControlInput::Pause},
});

inline auto menuControlInputs = makeArray<NamePair<MenuControlInput>>({
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

class RepeatControl {
  public:
    // input.cc
    bool update(bool down, float interval);

  private:
    bool lastDown_{false};
    float repeatTime_{0};
    int repeatCount_{0};
};

struct ControlState {
    bool up{false};
    bool down{false};
    bool left{false};
    bool right{false};
    bool forward{false};
    bool back{false};
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
            case ControlInput::Forward:
                return forward;
            case ControlInput::Back:
                return back;
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
        updateInputLevel(forward, forward_prev_, state.forward);
        updateInputLevel(back, back_prev_, state.back);
        updateInputLevel(fire, fire_prev_, state.fire);
        updateInputEdge(pause, pause_prev_, state.pause);
        return *this;
    }

  private:
    bool up_prev_{false};
    bool down_prev_{false};
    bool left_prev_{false};
    bool right_prev_{false};
    bool forward_prev_{false};
    bool back_prev_{false};
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

    MenuControlState& update(const MenuControlState& state, float interval) {
        updateInputEdge(exit, exit_prev_, state.exit);
        updateInputEdge(select, select_prev_, state.select);
        up = up_ramp_.update(state.up, interval);
        down = down_ramp_.update(state.down, interval);
        left = left_ramp_.update(state.left, interval);
        right = right_ramp_.update(state.right, interval);
        return *this;
    }

  private:
    bool exit_prev_;
    bool select_prev_;
    RepeatControl up_ramp_;
    RepeatControl down_ramp_;
    RepeatControl left_ramp_;
    RepeatControl right_ramp_;
};

using control_t = int;
constexpr static control_t controlNone = -1;

};  // namespace hiemalia

#endif  // M_CONTROLS_HH
