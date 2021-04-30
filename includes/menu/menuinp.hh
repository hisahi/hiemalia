/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menu/menuinp.hh: header file for menuinp.cc (input device menu)

#ifndef M_MENUINP_HH
#define M_MENUINP_HH

#include <string>

#include "defs.hh"
#include "menu.hh"
#include "mholder.hh"
#include "symbol.hh"

namespace hiemalia {
class MenuInputDevices : public Menu {
  public:
    std::string name() const noexcept { return name_; }
    std::string title() const noexcept { return "Input"; }

    void begin(GameState& state);
    void select(int index, symbol_t id);
    void end(GameState& state);

    DELETE_COPY(MenuInputDevices);
    DEFAULT_MOVE(MenuInputDevices);
    MenuInputDevices(MenuHandler& handler,
                     std::shared_ptr<ModuleHolder> holder);
    ~MenuInputDevices() noexcept = default;

  private:
    static inline const std::string name_ = "MenuInputDevices";
    std::shared_ptr<ModuleHolder> holder_;
};
};  // namespace hiemalia

#endif  // M_MENUINP_HH
