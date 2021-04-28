/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menu/menumain.hh: header file for menumain.cc (main menu)

#ifndef M_MENUMAIN_HH
#define M_MENUMAIN_HH

#include <array>
#include <string>

#include "cbuffer.hh"
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
    void renderSpecial(SplinterBuffer& sbuf, float interval);

    DELETE_COPY(MenuMain);
    DEFAULT_MOVE(MenuMain);
    MenuMain(MenuHandler& handler, const std::shared_ptr<ModuleHolder>& holder);
    ~MenuMain() noexcept;

  protected:
    void timedOut();

  private:
    static inline const std::string name_ = "MenuMain";
    std::shared_ptr<ModuleHolder> holder_;
    std::shared_ptr<const Model> tube_;
    Renderer2D rend2_;
    RendererText rendtext_;
    Renderer3D rend_;
    coord_t angle_{0};
    CircularBuffer<coord_t, 512> rots_;
    ShapeSheet logoSheet;
    SplinterBuffer copyright;
    int nextScreen_{0};

    void progress(float interval);
};
};  // namespace hiemalia

#endif  // M_MENUMAIN_HH
