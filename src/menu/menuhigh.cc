/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menuhigh.cc: implementation of high score table menu

#include "menu/menuhigh.hh"

#include "assets.hh"
#include "defs.hh"
#include "logger.hh"
#include "math.hh"
#include "menu/menuhelp.hh"
#include "menu/menuyn.hh"
#include "scores.hh"

namespace hiemalia {
static Shape highlight_ = Shape({ShapeFragment(
    Color{255, 255, 255, 0}, {-0.875, -0.035},
    {{0.875, -0.035}, {0.875, 0.035}, {-0.875, 0.035}, {-0.875, -0.035}})});

enum Item : symbol_t {
    Item_Reset = index_to_symbol(100),
    Item_Back = index_to_symbol(9999)
};

void MenuHighScore::begin(GameState& state) {
    option(MenuOption::label(index_to_symbol(0),
                             "       Loop/Stage                  "));
    option(MenuOption::spacer(symbol_none));
    listIndex_ = options_.size();
    for (int i = 0, n = HighScoreTable::size; i < n; ++i) {
        option(MenuOption::label(index_to_symbol(i + 1), ""));
    }
    option(MenuOption::spacer(symbol_none));
    if (!arcade()) {
        option(MenuOption::button(Item_Reset, "RESET"));
        option(MenuOption::button(Item_Back, "BACK"));
        defaultItem = Item_Back;
    }
    table_ = &state.highScores;
    redrawScores();
    duration(10);
}

void MenuHighScore::highlight(int index) { highlightIndex_ = index; }

void MenuHighScore::redrawScores() {
    if (!table_) return;
    HighScoreTable& table = *table_;
    for (int i = 0, n = table.entries.size(); i < n; ++i) {
        const auto& e = table.entries[i];
        std::string s =
            stringFormat("%.3s      %2d/%d      %12lu", e.name.data(), e.cycles,
                         e.endStage, e.score);
        options_[i + listIndex_].text = s;
        options_[i + listIndex_].dirty = true;
    }
}

void MenuHighScore::resetScores() {
    table_->resetScores();
    redrawScores();
    highlightIndex_ = -1;
}

void MenuHighScore::select(int index, symbol_t id) {
    if (arcade()) {
        closeMenu();
        return;
    }
    if (id == Item_Back) {
        closeMenu();
        return;
    } else if (id == Item_Reset) {
        confirm(Item_Reset, "RESET?", [&] { resetScores(); }, {});
    }
}

void MenuHighScore::end(GameState& state) {}

void MenuHighScore::renderSpecial(SplinterBuffer& sbuf, float interval) {
    bgr_ = wrapAngle(bgr_ + interval * 0.2f);
    rend3d_.renderModel(sbuf, Point3D(0, 0, 0.375),
                        Orient3D(bgr_, sin(bgr_ * 2), 0), Point3D(1, 1, 1),
                        *donut_);

    if (highlightIndex_ >= 0) {
        rend2d_.renderShapeColor(
            sbuf, 0, getMenuOptionY(highlightIndex_ + listIndex_),
            Color{255, 255, 0, static_cast<uint8_t>(176 + 64 * sin(t_))},
            highlight_);
        t_ = wrapAngle(t_ + interval);
    }
}

MenuHighScore::MenuHighScore(MenuHandler& handler,
                             const std::shared_ptr<ModuleHolder>& holder)
    : Menu(handler), holder_(holder), table_(nullptr) {
    rend3d_.setCamera(Point3D(0, 0, 0), Orient3D(0, 0, 0),
                      Point3D(1, 1, 1) * 0.25);
    donut_ = getGameModel(GameModel::Donut).model;
}

MenuHighScore::~MenuHighScore() noexcept {}

}  // namespace hiemalia
