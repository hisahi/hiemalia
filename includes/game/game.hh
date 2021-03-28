/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/game.hh: header file for the main game module (game/game.cc)

#ifndef M_GAME_GAME_HH
#define M_GAME_GAME_HH

#include <string>

#include "defs.hh"
#include "game/gamemsg.hh"
#include "game/world.hh"
#include "inherit.hh"
#include "lmodule.hh"
#include "menu.hh"
#include "model.hh"
#include "msg.hh"
#include "rend2d.hh"
#include "rend3d.hh"
#include "rendtext.hh"
#include "state.hh"

namespace hiemalia {

class GameMain : public LogicModule,
                 MessageHandler<GameMessage>,
                 MessageHandler<MenuMessage> {
   public:
    std::string name() const noexcept { return name_; }
    std::string role() const noexcept { return role_; }

    void gotMessage(const GameMessage& msg);
    void gotMessage(const MenuMessage& msg);
    bool run(GameState& state, float interval);

    DELETE_COPY(GameMain);
    GameMain(GameMain&& move) noexcept;
    GameMain& operator=(GameMain&& move) noexcept;
    GameMain();
    virtual ~GameMain() noexcept;

   private:
    static inline const std::string name_ = "GameMain";
    static inline const std::string role_ = "game module";

    std::unique_ptr<GameWorld> world_;
    Renderer2D r2d_;
    Renderer3D r3d_;
    RendererText font_;
    SplinterBuffer statusbar_;
    bool continue_{true};
    bool shouldBePaused_{false};
    bool paused_{false};
    coord_t moveSpeed;

    void startNewStage();
    void drawStatusBar();
    void positionCamera();
    void updateMoveSpeedInput(ControlState& controls, float delta);
    void pauseGame();
};
};  // namespace hiemalia

#endif  // M_GAME_GAME_HH
