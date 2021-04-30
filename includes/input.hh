/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// input.hh: header file for input engine module

#ifndef M_INPUT_HH
#define M_INPUT_HH

#include "config.hh"
#include "ibase.hh"
#include "inherit.hh"
#include "module.hh"
#include "msg.hh"
#include "state.hh"

namespace hiemalia {
class InputEngine : public Module {
  public:
    std::string name() const noexcept { return name_; }
    std::string role() const noexcept { return role_; }

    DELETE_COPY(InputEngine);

    InputEngine(InputEngine&& move) noexcept
        : Module(std::move(move)),
          controlsmenu_(std::move(move.controlsmenu_)),
          input_(std::move(move.input_)) {}
    InputEngine& operator=(InputEngine&& move) noexcept {
        Module::operator=(std::move(move));
        controlsmenu_ = std::move(move.controlsmenu_);
        input_ = std::move(move.input_);
        return *this;
    }
    InputEngine(const std::shared_ptr<HostModule>& host, GameState& state);
    ~InputEngine() noexcept {}

    bool hasInputDevice(InputDevice device) const;
    InputControlModule& getInputDevice(InputDevice device);
    void update(GameState& state, float interval);

  private:
    MenuControlState controlsmenu_;
    std::shared_ptr<InputModule> input_;
    static inline const std::string name_ = "InputEngine";
    static inline const std::string role_ = "input engine";
};

}  // namespace hiemalia

#endif  // M_INPUT_HH
