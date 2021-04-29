/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// menuhelp.cc: implementation of help screens

#include "menu/menuhelp.hh"

#include "assets.hh"
#include "audio.hh"
#include "hiemalia.hh"
#include "math.hh"

namespace hiemalia {

enum Item : symbol_t { Item_Music, Item_Sound, Item_Back };
static constexpr int rowCount = 24;

void MenuHelp::begin(GameState& state) {
    for (int i = 0; i < rowCount; ++i) {
        option(
            MenuOption::label(symbol_none, "012345678901234567890123456789"));
    }
    loadPage();
}

void MenuHelp::select(int index, symbol_t id) {
    if (info().arcade) {
        closeMenu();
        return;
    }
    pageRight(true);
}

void MenuHelp::end(GameState& state) {}

void MenuHelp::loadPage() {
    int i = 0;
    // ------------------------- 012345678901234567890123456789
    switch (page_) {
        case HelpPage::HowToPlay:
            options_.at(i++).text = "                              ";
            options_.at(i++).text = " Hiemalia is a tunnel-flying, ";
            options_.at(i++).text = " rail shoot 'em up. Your task ";
            options_.at(i++).text = "is to fly a ship through a set";
            options_.at(i++).text = "  of stages and get as high a ";
            options_.at(i++).text = "      score as possible.      ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "  The ship can be controlled  ";
            options_.at(i++).text = " with six controls which move ";
            options_.at(i++).text = "   it in the 3D space, with   ";
            options_.at(i++).text = "up/down/left/right, as well as";
            options_.at(i++).text = "forward/backward controls.  In";
            options_.at(i++).text = "addition, the ship is equipped";
            options_.at(i++).text = "with a powerful semi-automatic";
            options_.at(i++).text = "dual phaser system that can be";
            options_.at(i++).text = "operated with the fire button.";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "The game has four stages,  and";
            options_.at(i++).text = "loops endlessly,  getting more";
            options_.at(i++).text = "difficult with every loop. Get";
            options_.at(i++).text = "  the high score by shooting  ";
            options_.at(i++).text = "   enemies and by completing  ";
            options_.at(i++).text = "  stages as fast as you can.  ";
            options_.at(i++).text = "                              ";
            break;
        case HelpPage::Player:
            options_.at(i++).text = " STARSHIP 07-273 'HIEMALIA'   ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "   WEAPON    DUAL PHASER      ";
            options_.at(i++).text = "   ENGINE    HALL THRUSTERS   ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "  STRENGTHS   AGILITY         ";
            options_.at(i++).text = "              LOW PRICE       ";
            options_.at(i++).text = "              FUEL ECONOMY    ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "  WEAKNESSES  SHIELDS         ";
            options_.at(i++).text = "              LOOKS           ";
            options_.at(i++).text = "              DURABILITY      ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = " AVERAGE REVIEW SCORE   3.5/5 ";
            break;
        case HelpPage::HUD:
            options_.at(i++).text = "SCORE          HIGH SCORE (#1)";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "  H     H   U     U   DDDDD   ";
            options_.at(i++).text = "  H     H   U     U   D    D  ";
            options_.at(i++).text = "  H     H   U     U   D    D  ";
            options_.at(i++).text = "  HHHHHHH   U     U   D    D  ";
            options_.at(i++).text = "  H     H   U     U   D    D  ";
            options_.at(i++).text = "  H     H   U     U   D    D  ";
            options_.at(i++).text = "  H     H    UUUUU    DDDDD   ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "LIVES           LOOP * STAGE/4";
            break;
        case HelpPage::Stage:
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "  AVOID  OUTER  STAGE  WALLS  ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "    AVOID HITTING WALLS IN    ";
            options_.at(i++).text = "    GENERAL - YOUR SHIP IS    ";
            options_.at(i++).text = "        EASILY TOTALED        ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "     AVOID HITTING ENEMIES    ";
            options_.at(i++).text = "        (OR ENEMY FIRE)       ";
            options_.at(i++).text = "   YOUR SHIP CAREENS OUT OF   ";
            options_.at(i++).text = "  CONTROL IF HIT BY A BULLET! ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "    COLLECT  TIME  BONUSES    ";
            options_.at(i++).text = "    WITH SKILLFUL PILOTING    ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            break;
        case HelpPage::Enemies:
            options_.at(i++).text = "                              ";
            options_.at(i++).text = " THIS IS JUST A SAMPLE OF THE ";
            options_.at(i++).text = "ENEMIES YOU WILL  MEET ON YOUR";
            options_.at(i++).text = "          JOURNEY . . .       ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "CAN YOU COMPLETE  AT LEAST ONE";
            options_.at(i++).text = "LOOP THROUGH  ALL FOUR STAGES?";
            options_.at(i++).text = "                              ";
            options_.at(i++).text = "                              ";
            break;
        default:
            break;
    }
    dynamic_assert(i == rowCount, "fill in all rows");
    while (--i >= 0) options_[i].dirty = true;
    duration(10);
}

void MenuHelp::renderSpecial(SplinterBuffer& sbuf, float interval) {
    switch (page_) {
        case HelpPage::Player: {
            coord_t y = sin(rot_);
            y *= sgn(y) * y * 0.25;
            r3d_.renderModel(sbuf, Point3D(0, -1.0 / 24, 0.375),
                             Orient3D(rot_ * 3, y, 0), Point3D(1, 1, 1),
                             *playerShip_);
            break;
        }
        case HelpPage::Stage: {
            Point3D p = Point3D(0, 0, -0.5 - fmod(off_, stageSectionLength));
            Orient3D r = Orient3D(0, 0, 0);
            Point3D s = Point3D(1, 1, 1);
            for (unsigned i = 0; i < stageVisibility; ++i) {
                r3d_.renderModel(sbuf, p, r, s, *stageSection_);
                p.z += stageSectionLength;
            }
            break;
        }
        case HelpPage::Enemies: {
            r3d_.renderModel(
                sbuf, Point3D(0.3, -0.3, 0.875),
                Orient3D(rot_ * 3 + 2 * numbers::TAU<coord_t> / 3, -0.2, 0),
                Point3D(1, 1, 1), *enemy03_);
            r3d_.renderModel(sbuf, Point3D(0.2, 0.15, 0.75),
                             Orient3D(rot_ * 3, -0.2, 0), Point3D(1, 1, 1),
                             *enemy01_);
            r3d_.renderModel(
                sbuf, Point3D(-0.2, -0.1, 0.625),
                Orient3D(rot_ * 3 + numbers::TAU<coord_t> / 3, -0.2, 0),
                Point3D(1, 1, 1), *enemy02_);
            break;
        }
        default:
            break;
    }
    rot_ = wrapAngle(rot_ + interval);
    off_ = frac(off_ + (0.375 + 0.25 * sin(rot_)) * interval);
}

void MenuHelp::pageLeft() {
    if (info().arcade) {
        return;
    }
    sendMessage(AudioMessage::playSound(SoundEffect::MenuSelect));
    page_ = static_cast<HelpPage>(
        remainder(static_cast<int>(page_) - 1, numHelpPages));
    loadPage();
}

void MenuHelp::pageRight(bool bySelect) {
    if (info().arcade) {
        return;
    }
    sendMessage(AudioMessage::playSound(SoundEffect::MenuSelect));
    if (bySelect && static_cast<int>(page_) + 1 == numHelpPages) {
        closeMenu();
        return;
    }
    page_ = static_cast<HelpPage>(
        remainder(static_cast<int>(page_) + 1, numHelpPages));
    loadPage();
}

void MenuHelp::timedOut() {
    if (static_cast<int>(page_) + 1 == numHelpPages) {
        closeMenu();
        return;
    }
    page_ = static_cast<HelpPage>(
        remainder(static_cast<int>(page_) + 1, numHelpPages));
    loadPage();
}

MenuHelp::MenuHelp(MenuHelp&& move) noexcept
    : Menu(std::move(move)),
      rendtext_(std::move(move.rendtext_)),
      r2d_(std::move(move.r2d_)),
      r3d_(std::move(move.r3d_)),
      playerShip_(std::move(move.playerShip_)),
      stageSection_(std::move(move.stageSection_)),
      enemy01_(std::move(move.enemy01_)),
      enemy02_(std::move(move.enemy02_)),
      enemy03_(std::move(move.enemy03_)),
      page_(move.page_),
      rot_(move.rot_) {
    loadPage();
}

MenuHelp& MenuHelp::operator=(MenuHelp&& move) noexcept {
    Menu::operator=(std::move(move));
    rendtext_ = std::move(move.rendtext_);
    r2d_ = std::move(move.r2d_);
    r3d_ = std::move(move.r3d_);
    playerShip_ = std::move(move.playerShip_);
    stageSection_ = std::move(move.stageSection_);
    enemy01_ = std::move(move.enemy01_);
    enemy02_ = std::move(move.enemy02_);
    enemy03_ = std::move(move.enemy03_);
    page_ = move.page_;
    rot_ = move.rot_;
    return *this;
}

MenuHelp::MenuHelp(MenuHandler& handler) : Menu(handler) {
    r3d_.setCamera(Point3D(0, 0, 0), Orient3D(0, 0, 0),
                   Point3D(1, 1, 1) * 0.25);
    playerShip_ = getGameModel(GameModel::PlayerShip).model;
    stageSection_ = &getSectionById(0).model;
    enemy01_ = getGameModel(GameModel::EnemyGunboat).model;
    enemy02_ = getGameModel(GameModel::EnemyFighter).model;
    enemy03_ = getGameModel(GameModel::EnemyDestroyer).model;
}

MenuHelp::~MenuHelp() noexcept {}

}  // namespace hiemalia
