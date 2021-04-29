/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menumain.cc: implementation of MenuMain

#include "menu/menumain.hh"

#include "assets.hh"
#include "defs.hh"
#include "hiemalia.hh"
#include "load2d.hh"
#include "logic.hh"
#include "math.hh"
#include "menu/menuhelp.hh"
#include "menu/menuhigh.hh"
#include "menu/menuopt.hh"

namespace hiemalia {

enum Item : symbol_t {
    Item_Game,
    Item_Options,
    Item_Scores,
    Item_Help,
    Item_Exit
};

void MenuMain::begin(GameState& state) {
    if (!info().arcade) {
        option(MenuOption::spacer(symbol_none));
        option(MenuOption::spacer(symbol_none));
        option(MenuOption::spacer(symbol_none));
        option(MenuOption::spacer(symbol_none));
        option(MenuOption::button(Item_Game, "GAME"));
        option(MenuOption::button(Item_Options, "OPTIONS"));
        option(MenuOption::button(Item_Scores, "HIGH SCORES"));
        option(MenuOption::button(Item_Help, "HELP"));
        option(MenuOption::button(Item_Exit, "EXIT"));
        sendMessage(AudioMessage::playMusic(MusicTrack::Ambience));
    } else {
        switch (info().mainMenuMode) {
            case MainMenuEnterFlag::MainMenu:
                break;
            case MainMenuEnterFlag::HighScore:
                nextScreen_ = 3;
                break;
            case MainMenuEnterFlag::FromDemo:
                nextScreen_ = 0;
                break;
        }
        duration(10);
    }
}

void MenuMain::select(int index, symbol_t id) {
    switch (id) {
        case Item_Game:
            sendMessage(LogicMessage::startGame(holder_));
            closeMenu();
            break;
        case Item_Options:
            openMenu<MenuOptions>(holder_);
            break;
        case Item_Scores:
            openMenu<MenuHighScore>(holder_);
            break;
        case Item_Help:
            openMenu<MenuHelp>();
            break;
        case Item_Exit:
            sendMessage(HostMessage::quit());
            break;
    }
}

void MenuMain::end(GameState& state) {}

static const coord_t z_off = 0.125;

void MenuMain::progress(float interval) {
    rots_.push_front(
        wrapAngle(rots_[0] + (0.05 + 0.3 * std::sin(angle_)) * interval));
    angle_ = wrapAngle(angle_ + interval * 0.25);
}

void MenuMain::renderSpecial(SplinterBuffer& sbuf, float interval) {
    progress(interval);
    Point3D p(0, 0, 0);
    Point3D scale(1, 1, 1);
    for (size_t i = 0; i < (rots_.size() >> 3); ++i) {
        rend_.renderModel(sbuf, p, Orient3D(0, 0, rots_[i << 3]), scale,
                          *tube_);
        p.z += z_off;
    }
    rend2_.renderShapeColor(
        sbuf, 0, info().arcade ? 0 : -0.5,
        Color{255, 255, 255, static_cast<std::uint8_t>(192 + 60 * sin(angle_))},
        logoSheet[0]);
    sbuf.append(copyright);
}

MenuMain::MenuMain(MenuHandler& handler,
                   const std::shared_ptr<ModuleHolder>& holder)
    : Menu(handler), holder_(holder) {
    makeUncloseable();
    tube_ = getGameModel(GameModel::TitleCubeModel).model;
    rend_.setCamera(Point3D(0, 0, 0), Orient3D(0, 0, 0),
                    Point3D(0.25, 0.25, 0.25));
    for (size_t i = 0; i < rots_.capacity(); ++i) {
        progress(tickInterval);
    }
    logoSheet = load2D("logo.2d");
    rendtext_.setFont(getAssets().menuFont);
    rendtext_.drawTextLineCenter(copyright, 0, 0.875, Color{255, 255, 255, 128},
                                 "(C) HISAHI 2021", 0.75);
}

void MenuMain::timedOut() {
    if (nextScreen_ == 0) {
        openMenu<MenuHighScore>(holder_);
        nextScreen_ = 1;
    } else if (nextScreen_ == 1) {
        openMenu<MenuHelp>();
        nextScreen_ = 2;
    } else if (nextScreen_ == 2) {
        openMenu<MenuHighScore>(holder_);
        nextScreen_ = 3;
    } else if (nextScreen_ == 3) {
        duration(10);
        nextScreen_ = 4;
    } else if (nextScreen_ == 4) {
        sendMessage(LogicMessage::startDemo(holder_));
        closeMenu();
        // nextScreen_ = 5;
    }
}

MenuMain::~MenuMain() noexcept {}

}  // namespace hiemalia
