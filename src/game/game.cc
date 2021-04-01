/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/game.cc: implementation of GameMain

#include "game/game.hh"

#include "assets.hh"
#include "hiemalia.hh"
#include "logic.hh"
#include "math.hh"

namespace hiemalia {

static const coord_t defaultMoveSpeed = 1;
static const coord_t minimumMoveSpeed = 0.25;
static const coord_t maximumMoveSpeed = 2.5;
static const Color white{255, 255, 255, 255};
static const bool firstPerson = true;

GameMain::GameMain(GameMain&& move) noexcept
    : LogicModule(std::move(move)),
      world_(std::move(move.world_)),
      r2d_(std::move(move.r2d_)),
      r3d_(std::move(move.r3d_)),
      font_(std::move(move.font_)),
      statusbar_(std::move(move.statusbar_)),
      moveSpeed(move.moveSpeed),
      timer(move.timer) {}

GameMain& GameMain::operator=(GameMain&& move) noexcept {
    LogicModule::operator=(std::move(move));
    world_ = std::move(move.world_);
    r2d_ = std::move(move.r2d_);
    r3d_ = std::move(move.r3d_);
    font_ = std::move(move.font_);
    statusbar_ = std::move(move.statusbar_);
    return *this;
}

GameMain::GameMain() : world_(std::make_unique<GameWorld>()) {
    font_.setFont(getAssets().menuFont);
    moveSpeed = defaultMoveSpeed;
    timer = 0;
}

GameMain::~GameMain() noexcept {}

static void drawScore(SplinterBuffer& sbuf, RendererText& font, coord_t x,
                      coord_t y, unsigned score) {
    std::string s = std::to_string(score);
    s.insert(s.begin(), 9 - s.size(), ' ');
    font.renderTextLine(sbuf, x - font.getTextWidth(s) / 2, y, white, s);
}

static void drawLives(SplinterBuffer& sbuf, RendererText& font, coord_t x,
                      coord_t y, unsigned lives) {
    std::string s = std::to_string(lives);
    s.insert(s.begin(), 2 - s.size(), ' ');
    font.renderTextLine(sbuf, x - font.charWidth() * 4, y, white, "LIVES");
    font.renderTextLine(sbuf, x + font.charWidth() * 2, y, white, "*");
    font.renderTextLine(sbuf, x + font.charWidth() * 3, y, white, s);
}

void GameMain::drawStatusBar() {
    statusbar_.clear();
    drawScore(statusbar_, font_, -0.5, -0.9375, world_->score);
    drawScore(statusbar_, font_, 0.5, -0.9375, world_->highScore);
    drawLives(statusbar_, font_, -0.5, 0.9375, world_->lives);
}

void GameMain::positionCamera() {}

void GameMain::startNewStage() { world_->startNewStage(); }

void GameMain::pauseGame() {
    if (!paused_) {
        shouldBePaused_ = true;
        paused_ = true;
        sendMessage(LogicMessage::pauseMenu());
        sendMessage(AudioMessage::playSound(SoundEffect::Pause));
    }
}

void GameMain::gotMessage(const GameMessage& msg) {
    switch (msg.type) {
        case GameMessageType::AddScore:
            break;
        case GameMessageType::UpdateCamera:
            positionCamera();
            break;
        case GameMessageType::UpdateStatus:
            drawStatusBar();
            break;
        case GameMessageType::PauseGame:
            pauseGame();
            break;
        case GameMessageType::ContinueGame:
            shouldBePaused_ = false;
            break;
        case GameMessageType::StageComplete:
            doStageComplete();
            break;
        case GameMessageType::ExitGame:
            doExitGame();
            break;
    }
}

void GameMain::doExitGame() {
    continue_ = false;
    sendMessage(HostMessage::mainMenu());
}

void GameMain::gotMessage(const MenuMessage& msg) {
    if (!paused_ && msg.type == MenuMessageType::MenuExit) {
        pauseGame();
    }
}

void GameMain::updateMoveSpeedInput(ControlState& inputs, float delta) {
    if (inputs.back) {
        moveSpeed = std::max<coord_t>(
            moveSpeed + (minimumMoveSpeed - defaultMoveSpeed) * delta,
            minimumMoveSpeed);
    } else if (inputs.forward) {
        moveSpeed = std::min<coord_t>(
            moveSpeed + (maximumMoveSpeed - defaultMoveSpeed) * delta * 2,
            maximumMoveSpeed);
    } else if (moveSpeed < defaultMoveSpeed) {
        moveSpeed = std::min<coord_t>(
            defaultMoveSpeed,
            moveSpeed + (defaultMoveSpeed - minimumMoveSpeed) * delta * 2);
    } else if (moveSpeed > defaultMoveSpeed) {
        moveSpeed = std::max<coord_t>(
            defaultMoveSpeed,
            moveSpeed + (defaultMoveSpeed - maximumMoveSpeed) * delta);
    }
}

coord_t moveSpeed{0.5};

void GameMain::drawObject(GameState& state, float interval,
                          const ObjectPtr& obj) {
    obj->render(state.sbuf, r3d_);
}

bool GameMain::updateObject(GameState& state, float interval,
                            const ObjectPtr& obj) {
    bool ok = obj->update(*world_, interval);
    if (ok) drawObject(state, interval, obj);
    return ok;
}

void GameMain::doStageStart() {
    GameWorld& w = *world_;
    stageStartTimer = 5;
    sendMessage(AudioMessage::playMusic(MusicTrack::StageStart));
    textscreen_.clear();
    font_.drawTextLineCenter(textscreen_, 0, -0.25, white,
                             "STAGE " + std::to_string(w.stageNum), 2);
}

static bool timerCrosses(float timer, float interval, float threshold) {
    return timer > threshold && timer - interval <= threshold;
}

static const ModelPoint c_scale = ModelPoint(0.25, 0.25, 0.25);
static const coord_t p_fpx = 0;
static const coord_t p_fpy = -0.01171875;
static const coord_t p_fpz = -0.03515625;

void GameMain::doStageStartTick(GameState& state, float interval) {
    GameWorld& w = *world_;
    static const coord_t backDistance = -0.25;
    if (stageStartTimer < 4) {
        float prog = unlerp(4.0f, stageStartTimer, 1.0f);
        if (prog >= 1) {
            prog = unlerp(1.0f, stageStartTimer, 0.0f);
            r3d_.setCamera(sqerpPoint(ModelPoint(0, 0, backDistance), prog,
                                      ModelPoint(p_fpx, p_fpy, p_fpz)),
                           w.getPlayer().rot, c_scale);
        } else {
            float radius = 1 - prog;
            float angle = static_cast<float>(prog * numbers::TAU);
            float r2 = radius * radius;
            float s = std::sin(angle);
            float c = std::cos(angle);
            /*r3d_.setCamera(
                ModelPoint(4 * r2 * s, 4 * r2 * c,
                           lerp<coord_t>(0, prog, backDistance)),
                Rotation3D(-radius * numbers::PI / 2 * s,
                           -radius * numbers::PI / 2 * c, radius / 4),
                c_scale);*/
            r3d_.setCamera(
                ModelPoint(4 * r2 * s, -r2 * 2,
                           lerp<coord_t>(0, prog, backDistance) - 4 * r2 * c),
                Rotation3D(-std::atan2(4 * r2 * s, 4 * r2 * c), -r2,
                           -radius / 4),
                c_scale);
        }
        w.drawStage(state.sbuf, r3d_);
        w.renderPlayer(state.sbuf, r3d_);
    }
    if (timerCrosses(stageStartTimer, interval, 1)) {
        font_.drawTextLineCenter(textscreen_, 0, 0.25, white, "START!", 2);
    }
    stageStartTimer -= interval;
    if (stageStartTimer <= 0) {
        textscreen_.clear();
    }
}

void GameMain::doStageComplete() {
    timer = 8;
    stageStartTimer = 0;
    stageComplete_ = true;
    sendMessage(AudioMessage::fadeOutMusic());
}

void GameMain::doStageCompleteTick(GameState& state, float interval) {
    GameWorld& w = *world_;
    w.drawStage(state.sbuf, r3d_);
    w.renderPlayer(state.sbuf, r3d_);
    drawObjects(state, interval, w.objects);
    drawObjects(state, interval, w.enemyBullets);
    drawObjects(state, interval, w.playerBullets);
    timer -= interval;
    stageStartTimer = 0;
    if (timer <= 0) {
        textscreen_.clear();
        stageComplete_ = false;
        w.nextStage = true;
    }
}

void GameMain::doGameOver() {
    // game over
    gameOver_ = true;
    timer = 5;
    font_.drawTextLineCenter(textscreen_, 0, 0, white, "GAME OVER", 2);
    sendMessage(AudioMessage::playMusic(MusicTrack::GameOver));
    stageComplete_ = false;
}

bool GameMain::run(GameState& state, float interval) {
    static constexpr coord_t Y = 0.875;
    static const Rotation3D c_rot = Rotation3D(0, 0, 0);
    static const Rotation3D c_trot = Rotation3D(
        radians<coord_t>(-60), radians<coord_t>(30), radians<coord_t>(-30));
    if (!continue_) return false;
    if (gameOver_) {
        state.sbuf.append(textscreen_);
        state.sbuf.append(statusbar_);
        timer -= interval;
        if (timer > 0) return true;
        doExitGame();
        return false;
    }

    GameWorld& w = *world_;

    if (w.nextStage) {
        w.nextStage = false;
        startNewStage();
        doStageStart();
        drawStatusBar();
    }

    paused_ = shouldBePaused_;
    if (!paused_ && state.controls.pause) pauseGame();
    if (paused_) {
        // I am paused
    } else if (stageStartTimer > 0) {
        state.sbuf.push(Splinter(SplinterType::BeginClipCenter, -Y, +Y));
        doStageStartTick(state, interval);
        state.sbuf.push(Splinter(SplinterType::EndClip, 0, 0));
    } else if (stageComplete_) {
        state.sbuf.push(Splinter(SplinterType::BeginClipCenter, -Y, +Y));
        doStageCompleteTick(state, interval);
        state.sbuf.push(Splinter(SplinterType::EndClip, 0, 0));
    } else {
        state.sbuf.push(Splinter(SplinterType::BeginClipCenter, -Y, +Y));
        if (w.isPlayerAlive()) {
            auto& plr = w.player;
            plr->updateInput(state.controls);
            updateMoveSpeedInput(state.controls, interval);
            w.moveSpeed = moveSpeed;
            w.moveForward(interval * w.moveSpeed);
        } else {
            moveSpeed = defaultMoveSpeed;
        }
        bool playerAlive = w.runPlayer(interval);
        const ModelPoint& p = w.getPlayerPosition();
        w.drawStage(state.sbuf, r3d_);
        if (w.isPlayerAlive()) {
            auto& plr = w.player;
            if (firstPerson) {  // first person view
                r3d_.setCamera(
                    ModelPoint(p.x + p_fpx, p.y + p_fpy, p.z + p_fpz), plr->rot,
                    c_scale);
                // crosshair
                state.sbuf.push(Splinter(SplinterType::BeginShape, -0.03125,
                                         -0.03125, Color{255, 255, 0, 160}));
                state.sbuf.push(
                    Splinter(SplinterType::EndShapePoint, 0.03125, 0.03125));
                state.sbuf.push(Splinter(SplinterType::BeginShape, 0.03125,
                                         -0.03125, Color{255, 255, 0, 160}));
                state.sbuf.push(
                    Splinter(SplinterType::EndShapePoint, -0.03125, 0.03125));
            } else  // third person view
                r3d_.setCamera(ModelPoint(p.x, p.y, -0.25 + w.progress_f),
                               c_rot, c_scale);
        } else {
            r3d_.setCamera(ModelPoint(p.x + 0.25, p.y - 0.25, p.z - 0.25),
                           c_trot, c_scale);
        }
        processObjects(state, interval, w.objects);
        processObjects(state, interval, w.enemyBullets);
        processObjects(state, interval, w.playerBullets);
        if (playerAlive) {
            w.renderPlayer(state.sbuf, r3d_);
        } else if (!w.respawn()) {
            doGameOver();
        }
        state.sbuf.push(Splinter(SplinterType::EndClip, 0, 0));
    }
    state.sbuf.append(textscreen_);
    state.sbuf.append(statusbar_);
    return true;
}

}  // namespace hiemalia
