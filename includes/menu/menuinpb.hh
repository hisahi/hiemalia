/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menu/menuinpb.hh: header file for menuinp.cc (input controls menu)

#ifndef M_MENUINPB_HH
#define M_MENUINPB_HH

#include <string>
#include <variant>

#include "buttons.hh"
#include "defs.hh"
#include "ibase.hh"
#include "menu.hh"
#include "mholder.hh"
#include "namepair.hh"
#include "symbol.hh"

namespace hiemalia {

enum class InputMenuMessageType { NewButtonSet };

struct InputMenuMessage {
    InputMenuMessageType type;
    std::variant<ControlInput> value;

    inline static InputMenuMessage newButtonSet(ControlInput ctrl) {
        return InputMenuMessage(InputMenuMessageType::NewButtonSet, ctrl);
    }

    inline ControlInput button() const { return std::get<ControlInput>(value); }

  private:
    InputMenuMessage(InputMenuMessageType type) : type(type) {}
    InputMenuMessage(InputMenuMessageType type, ControlInput ctrl)
        : type(type), value(ctrl) {}
};

class MenuInputControls : public Menu, MessageHandler<InputMenuMessage> {
  public:
    std::string name() const noexcept { return name_; }
    std::string title() const noexcept;

    void begin(GameState& state);
    void select(int index, symbol_t id);
    void end(GameState& state);

    void gotMessage(const InputMenuMessage& msg);

    DELETE_COPY(MenuInputControls);
    MenuInputControls(MenuInputControls&& move) noexcept;
    MenuInputControls& operator=(MenuInputControls&& move) noexcept;
    MenuInputControls(MenuHandler& handler,
                      const std::shared_ptr<ModuleHolder>& holder,
                      NamePair<InputDevice> pair, InputControlModule& module);
    ~MenuInputControls() noexcept;

  private:
    static inline const std::string name_ = "MenuInputControls";
    std::shared_ptr<ModuleHolder> holder_;
    std::string title_;
    InputDevice device_;
    std::reference_wrapper<InputControlModule> module_;
    int settingButton_{-1};

    std::string getControlText(ControlInput input);
};
};  // namespace hiemalia

#endif  // M_MENUINPB_HH
