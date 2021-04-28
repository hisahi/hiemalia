/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menu/menuhelp.hh: header file for menuhelp.cc (help screens)

#ifndef M_MENUHELP_HH
#define M_MENUHELP_HH

#include <array>
#include <string>

#include "defs.hh"
#include "menu.hh"
#include "rend3d.hh"
#include "symbol.hh"

namespace hiemalia {
enum class HelpPage { HowToPlay, Player, HUD, Stage, Enemies };
constexpr int numHelpPages = 5;

class MenuHelp : public Menu {
  public:
    std::string name() const noexcept { return name_; }
    std::string title() const noexcept { return "Help"; }

    void begin(GameState& state);
    void select(int index, symbol_t id);
    void end(GameState& state);
    void renderSpecial(SplinterBuffer& sbuf, float interval);
    void pageLeft();
    void pageRight(bool bySelect);

    DELETE_COPY(MenuHelp);
    MenuHelp(MenuHelp&& move) noexcept;
    MenuHelp& operator=(MenuHelp&& move) noexcept;
    MenuHelp(MenuHandler& handler);
    ~MenuHelp() noexcept;

  protected:
    void timedOut();

  private:
    static inline const std::string name_ = "MenuHelp";
    RendererText rendtext_;
    Renderer2D r2d_;
    Renderer3D r3d_;
    std::shared_ptr<Model> playerShip_;
    const Model* stageSection_;
    std::shared_ptr<Model> enemy01_;
    std::shared_ptr<Model> enemy02_;
    std::shared_ptr<Model> enemy03_;
    HelpPage page_{HelpPage::HowToPlay};
    coord_t rot_{0};
    coord_t off_{0};
    void loadPage();
};
};  // namespace hiemalia

#endif  // M_MENUHELP_HH
