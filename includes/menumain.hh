/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menumain.hh: header file for menumain.cc (main menu)

#ifndef M_MENUMAIN_HH
#define M_MENUMAIN_HH

#include <array>
#include <string>

#include "defs.hh"
#include "menu.hh"
#include "mholder.hh"
#include "rend3d.hh"
#include "symbol.hh"

namespace hiemalia {
class MenuMain : public Menu {
   public:
    std::string name() const noexcept { return name_; }
    std::string title() const noexcept { return ""; }

    void begin(GameState& state);
    void select(int index, symbol_t id);
    void end(GameState& state);
    void specialRender(SplinterBuffer& sbuf, float interval);

    DELETE_COPY(MenuMain);
    MenuMain(MenuMain&& move) noexcept;
    MenuMain& operator=(MenuMain&& move) noexcept;
    MenuMain(MenuHandler& handler, const std::shared_ptr<ModuleHolder>& holder);
    ~MenuMain() noexcept;

   private:
    static inline const std::string name_ = "MenuMain";
    std::shared_ptr<ModuleHolder> holder_;
    std::shared_ptr<const Model> tube_;
    Renderer2D rend2_;
    Renderer3D rend_;
    coord_t angle_{0};
    std::array<coord_t, 512> rots_;
    ShapeSheet logoSheet;
};
};  // namespace hiemalia

#endif  // M_MENUMAIN_HH
