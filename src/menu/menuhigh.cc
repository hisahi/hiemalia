/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menuhigh.cc: implementation of high score table menu

#include "menu/menuhigh.hh"

#include "defs.hh"
#include "logger.hh"
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
                             "      Round/Stage                  "));
    option(MenuOption::spacer(symbol_none));
    listIndex_ = options_.size();
    for (int i = 0, n = HighScoreTable::size; i < n; ++i) {
        option(MenuOption::label(index_to_symbol(i + 1), ""));
    }
    option(MenuOption::spacer(symbol_none));
    option(MenuOption::button(Item_Reset, "RESET"));
    option(MenuOption::button(Item_Back, "BACK"));
    defaultItem = Item_Back;
    table_ = &state.highScores;
    redrawScores();
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
    if (id == Item_Back) {
        closeMenu();
        return;
    } else if (id == Item_Reset) {
        confirm(Item_Reset, "RESET?", [&] { resetScores(); }, {});
    }
}

void MenuHighScore::end(GameState& state) {}

void MenuHighScore::specialRender(SplinterBuffer& sbuf, float interval) {
    if (highlightIndex_ >= 0) {
        rend2d_.renderShapeColor(sbuf, 0,
                                 getMenuOptionY(highlightIndex_ + listIndex_),
                                 Color{255, 255, 0, 224}, highlight_);
    }
}

MenuHighScore::MenuHighScore(MenuHighScore&& move) noexcept
    : Menu(std::move(move)),
      holder_(std::move(move.holder_)),
      table_(move.table_),
      rend2d_(std::move(move.rend2d_)) {}

MenuHighScore& MenuHighScore::operator=(MenuHighScore&& move) noexcept {
    Menu::operator=(std::move(move));
    holder_ = std::move(move.holder_);
    table_ = move.table_;
    rend2d_ = std::move(move.rend2d_);
    return *this;
}

MenuHighScore::MenuHighScore(MenuHandler& handler,
                             const std::shared_ptr<ModuleHolder>& holder)
    : Menu(handler), holder_(holder), table_(nullptr) {}

MenuHighScore::~MenuHighScore() noexcept {}

}  // namespace hiemalia
