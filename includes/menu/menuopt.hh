/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menu/menuopt.hh: header file for menuopt.cc (option menu)

#ifndef M_MENUOPT_HH
#define M_MENUOPT_HH

#include <string>

#include "defs.hh"
#include "menu.hh"
#include "mholder.hh"
#include "symbol.hh"

namespace hiemalia {
class MenuOptions : public Menu {
  public:
    std::string name() const noexcept { return name_; }
    std::string title() const noexcept { return "Options"; }

    void begin(GameState& state);
    void select(int index, symbol_t id);
    void end(GameState& state);

    DELETE_COPY(MenuOptions);
    DEFAULT_MOVE(MenuOptions);
    MenuOptions(MenuHandler& handler,
                const std::shared_ptr<ModuleHolder>& holder);
    ~MenuOptions() noexcept;

  private:
    static inline const std::string name_ = "MenuOptions";
    std::shared_ptr<ModuleHolder> holder_;
};
};  // namespace hiemalia

#endif  // M_MENUOPT_HH
