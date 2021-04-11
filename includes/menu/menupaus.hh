/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menu/menupaus.hh: header file for menupaus.cc (pause menu)

#ifndef M_MENUPAUS_HH
#define M_MENUPAUS_HH

#include <string>

#include "defs.hh"
#include "menu.hh"
#include "mholder.hh"
#include "symbol.hh"

namespace hiemalia {
class MenuPause : public Menu {
  public:
    std::string name() const noexcept { return name_; }
    std::string title() const noexcept { return "Paused"; }

    void begin(GameState& state);
    void select(int index, symbol_t id);
    void end(GameState& state);

    DELETE_COPY(MenuPause);
    DEFAULT_MOVE(MenuPause);
    MenuPause(MenuHandler& handler);
    ~MenuPause() noexcept;

  private:
    static inline const std::string name_ = "MenuPause";
    bool unpause_{true};
};
};  // namespace hiemalia

#endif  // M_MENUPAUS_HH
