/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menu/menuyn.hh: header file for menuyn.cc (yes/no menu)

#ifndef M_MENUYN_HH
#define M_MENUYN_HH

#include <string>

#include "defs.hh"
#include "menu.hh"
#include "symbol.hh"

namespace hiemalia {
class MenuYesNo : public Menu {
  public:
    std::string name() const noexcept { return name_; }
    std::string title() const noexcept { return title_; }

    void begin(GameState& state);
    void select(int index, symbol_t id);
    void end(GameState& state);

    DELETE_COPY(MenuYesNo);
    MenuYesNo(MenuYesNo&& move) noexcept;
    MenuYesNo& operator=(MenuYesNo&& move) noexcept;
    MenuYesNo(MenuHandler& handler, symbol_t id, const std::string& title);
    ~MenuYesNo() noexcept;

  private:
    static inline const std::string name_ = "MenuYesNo";
    symbol_t myId_;
    std::string title_;
    bool yes_{false};
    bool no_{false};
};
};  // namespace hiemalia

#endif  // M_MENUYN_HH
