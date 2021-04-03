/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/game.hh: header file for the main game module (game/game.cc)

#ifndef M_GAME_GAME_HH
#define M_GAME_GAME_HH

#include <algorithm>
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
    SplinterBuffer textscreen_;
    bool init_{false};
    bool continue_{true};
    bool shouldBePaused_{false};
    bool stageComplete_{false};
    bool paused_{false};
    bool gameOver_{false};
    float timer;
    coord_t objectLateZ;
    float stageStartTimer{0};

    void doInit(GameState& state);
    void startNewStage();
    void drawStatusBar();
    void positionCamera();
    void pauseGame();
    void doStageStart();
    void doStageStartTick(GameState& state, float interval);
    void doStageComplete();
    void doStageCompleteTick(GameState& state, float interval);
    void doExitGame();
    void doGameOver();
    void drawObject(GameState& state, float interval, const ObjectPtr& obj);
    bool updateObject(GameState& state, float interval, const ObjectPtr& obj);

    template <typename T>
    void drawObjects(GameState& state, float interval, ObjectListBase<T>& v) {
        std::for_each(v.begin(), v.end(), [&](const ObjectPtr& obj) -> void {
            drawObject(state, interval, obj);
        });
    }
    template <typename T>
    void processObjects(GameState& state, float interval,
                        ObjectListBase<T>& v) {
        v.erase(std::remove_if(v.begin(), v.end(),
                               [&](const ObjectPtr& obj) -> bool {
                                   return !updateObject(state, interval, obj);
                               }),
                v.end());
    }
};
};  // namespace hiemalia

#endif  // M_GAME_GAME_HH
