/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menu/menuvid.hh: header file for menuvid.cc (video option menu)

#ifndef M_MENUVID_HH
#define M_MENUVID_HH

#include <string>

#include "defs.hh"
#include "menu.hh"
#include "mholder.hh"
#include "symbol.hh"
#include "video.hh"

namespace hiemalia {
class MenuVideoOptions : public Menu {
  public:
    std::string name() const noexcept { return name_; }
    std::string title() const noexcept { return "Video"; }

    void begin(GameState& state);
    void select(int index, symbol_t id);
    void end(GameState& state);

    DELETE_COPY(MenuVideoOptions);
    DEFAULT_MOVE(MenuVideoOptions);
    MenuVideoOptions(MenuHandler& handler,
                     std::shared_ptr<ModuleHolder> holder);
    ~MenuVideoOptions() noexcept = default;

  private:
    static inline const std::string name_ = "MenuVideoOptions";
    std::shared_ptr<ModuleHolder> holder_;
};
};  // namespace hiemalia

#endif  // M_MENUVID_HH
