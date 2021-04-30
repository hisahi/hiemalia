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
#include "game/demo.hh"
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
    DEFAULT_MOVE(GameMain);
    GameMain(const ConfigSectionPtr<GameConfig>& config,
             const std::shared_ptr<DemoFile>& demo);
    virtual ~GameMain() noexcept = default;

  private:
    static inline const std::string name_ = "GameMain";
    static inline const std::string role_ = "game module";

    std::unique_ptr<GameWorld> world_;
    ConfigSectionPtr<GameConfig> config_;
    std::shared_ptr<DemoFile> demo_;
    Renderer2D r2d_;
    Renderer3D r3d_;
    RendererText font_;
    SplinterBuffer statusbar_;
    SplinterBuffer textscreen_;
    SplinterBuffer blink_;
    coord_t cameraShake_{0};
    coord_t cameraShakeSpeed_{0};
    coord_t cameraShakeTime_{0};
    Point3D shake0_{0, 0, 0};
    Point3D shake1_{0, 0, 0};
    bool init_{false};
    bool arcade_{false};
    bool freePlay_{false};
    bool running_{true};
    bool continue_{false};
    bool shouldBePaused_{false};
    bool stageComplete_{false};
    bool gameComplete_{false};
    bool paused_{false};
    bool gameOver_{false};
    bool instaExit_{false};
    float timer;
    coord_t objectLateZ;
    float stageStartTimer{0};
    float stageTime{0};
    unsigned bonus_{0};
    int bonusIndex_{0};
    int continueResponse_{0};
    LoadedGameModel ring_;
    coord_t ringRot_{0};
    float halt_{0};

    void doInit(GameState& state);
    void startNewStage();
    void drawStatusBar();
    void positionCamera();
    void pauseGame();
    void doStageStart();
    void doStageStartTick(GameState& state, float interval);
    void doStageComplete();
    void doStageCompleteTick(GameState& state, float interval);
    void doContinuePrompt(int credits);
    void doContinuePromptTick(GameState& state, float interval);
    void doGameComplete();
    void doGameCompleteTick(GameState& state, float interval);
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
