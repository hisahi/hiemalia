/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menumain.hh: header file for menumain.cc (main menu)

#ifndef M_MENUMAIN_HH
#define M_MENUMAIN_HH

#include <string>

#include "defs.hh"
#include "menu.hh"
#include "symbol.hh"

namespace hiemalia {
class MenuMain : public Menu {
   public:
    std::string name() const noexcept { return name_; }

    void begin(GameState& state);
    void select(int index, symbol_t id);
    void end(GameState& state);

    DELETE_COPY(MenuMain);
    MenuMain(MenuMain&& move) noexcept;
    MenuMain& operator=(MenuMain&& move) noexcept;
    MenuMain();
    ~MenuMain() noexcept;

   private:
    static inline const std::string name_ = "MenuMain";
};
};  // namespace hiemalia

#endif  // M_MENUMAIN_HH
