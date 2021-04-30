/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menu/menugame.hh: header file for menugame.cc (game option menu)

#ifndef M_MENUGAME_HH
#define M_MENUGAME_HH

#include <string>

#include "audio.hh"
#include "defs.hh"
#include "menu.hh"
#include "mholder.hh"
#include "symbol.hh"

namespace hiemalia {
class MenuGameOptions : public Menu {
  public:
    std::string name() const noexcept { return name_; }
    std::string title() const noexcept { return "Game"; }

    void begin(GameState& state);
    void select(int index, symbol_t id);
    void end(GameState& state);

    DELETE_COPY(MenuGameOptions);
    DEFAULT_MOVE(MenuGameOptions);
    MenuGameOptions(MenuHandler& handler, std::shared_ptr<ModuleHolder> holder);
    ~MenuGameOptions() noexcept = default;

  private:
    static inline const std::string name_ = "MenuGameOptions";
    std::shared_ptr<ModuleHolder> holder_;
};
};  // namespace hiemalia

#endif  // M_MENUGAME_HH
