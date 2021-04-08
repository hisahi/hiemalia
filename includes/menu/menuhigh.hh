/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menu/menuhigh.hh: header file for menuhigh.cc (highscore table)

#ifndef M_MENUHIGH_HH
#define M_MENUHIGH_HH

#include <string>

#include "defs.hh"
#include "menu.hh"
#include "mholder.hh"
#include "symbol.hh"

namespace hiemalia {
class MenuHighScore : public Menu {
  public:
    std::string name() const noexcept { return name_; }
    std::string title() const noexcept { return "High Scores"; }

    void begin(GameState& state);
    void select(int index, symbol_t id);
    void end(GameState& state);
    void specialRender(SplinterBuffer& sbuf, float interval);
    void highlight(int index);

    DELETE_COPY(MenuHighScore);
    MenuHighScore(MenuHighScore&& move) noexcept;
    MenuHighScore& operator=(MenuHighScore&& move) noexcept;
    MenuHighScore(MenuHandler& handler,
                  const std::shared_ptr<ModuleHolder>& holder);
    ~MenuHighScore() noexcept;

  private:
    static inline const std::string name_ = "MenuHighScore";
    std::shared_ptr<ModuleHolder> holder_;
    void redrawScores();
    void resetScores();
    HighScoreTable* table_;
    Renderer2D rend2d_;
    int listIndex_{0};
    int highlightIndex_{-1};
};
};  // namespace hiemalia

#endif  // M_MENUHIGH_HH
