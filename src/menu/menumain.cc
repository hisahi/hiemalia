/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menumain.cc: implementation of MenuMain

#include "menumain.hh"

#include "assets.hh"
#include "defs.hh"
#include "hiemalia.hh"
#include "load2d.hh"
#include "logic.hh"
#include "math.hh"
#include "menuopt.hh"

namespace hiemalia {

enum Item : symbol_t {
    Item_Game,
    Item_Options,
    Item_Scores,
    Item_Help,
    Item_Exit
};

void MenuMain::begin(GameState& state) {
    option(MenuOption::spacer(symbol_none));
    option(MenuOption::spacer(symbol_none));
    option(MenuOption::spacer(symbol_none));
    option(MenuOption::spacer(symbol_none));
    option(MenuOption::button(Item_Game, "GAME"));
    option(MenuOption::button(Item_Options, "OPTIONS"));
    option(MenuOption::button(Item_Scores, "HIGH SCORES"));
    option(MenuOption::button(Item_Help, "HELP"));
    option(MenuOption::button(Item_Exit, "EXIT"));
}

void MenuMain::select(int index, symbol_t id) {
    switch (id) {
        case Item_Game:
            sendMessage(LogicMessage::startGame());
            closeMenu();
            break;
        case Item_Options:
            openMenu<MenuOptions>(holder_);
            break;
        case Item_Scores:
            break;
        case Item_Help:
            break;
        case Item_Exit:
            sendMessage(HostMessage::quit());
            break;
    }
}

void MenuMain::end(GameState& state) {}

static const coord_t z_off = 0.125;

void MenuMain::specialRender(SplinterBuffer& sbuf, float interval) {
    std::copy_backward(rots_.begin(), rots_.end() - 1, rots_.end());
    rots_[0] = wrapAngle(rots_[0] + (0.05 + 0.3 * std::sin(angle_)) * interval);
    angle_ = wrapAngle(angle_ + interval * 0.25);
    coord_t z = 0;
    ModelPoint scale(1, 1, 1);
    for (size_t i = 0; i < (rots_.size() >> 3); ++i) {
        rend_.renderModel(sbuf, ModelPoint(0, 0, z),
                          Rotation3D(0, 0, rots_[i << 3]), scale, *tube_);
        z += z_off;
    }
    rend2_.renderShapeColor(
        sbuf, 0, -0.5,
        Color{255, 255, 255,
              static_cast<std::uint8_t>(192 + 60 * std::sin(angle_))},
        logoSheet[0]);
}

MenuMain::MenuMain(MenuHandler& handler,
                   const std::shared_ptr<ModuleHolder>& holder)
    : Menu(handler), holder_(holder) {
    makeUncloseable();
    tube_ = getGameModel(GameModel::TitleCubeModel);
    rend_.setCamera(ModelPoint(0, 0, 0), Rotation3D(0, 0, 0),
                    ModelPoint(0.25, 0.25, 0.25));
    for (size_t i = 0; i < rots_.size(); ++i) {
        rots_[i] = i * (numbers::PI / 2048);
    }
    logoSheet = load2D("logo.2d");
}

MenuMain::~MenuMain() noexcept {}

MenuMain::MenuMain(MenuMain&& move) noexcept
    : Menu(std::move(move)),
      holder_(std::move(move.holder_)),
      tube_(std::move(move.tube_)) {}

MenuMain& MenuMain::operator=(MenuMain&& move) noexcept {
    Menu::operator=(std::move(move));
    holder_ = std::move(move.holder_);
    tube_ = std::move(move.tube_);
    return *this;
}

}  // namespace hiemalia
