/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// buttons.hh: header file for control setups

#ifndef M_BUTTONS_HH
#define M_BUTTONS_HH

#include <array>
#include <stdexcept>
#include <unordered_map>
#include <utility>

#include "config.hh"
#include "controls.hh"
#include "logger.hh"
#include "module.hh"

namespace hiemalia {
class ButtonSetup : public ConfigSection {
   public:
    ButtonSetup(const Module& input, NamePair<InputDevice> pair) noexcept
        : ConfigSection(input.name() + ":" + pair.name),
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
        return const_cast<control_t&>(std::as_const(*this).control(input));
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
