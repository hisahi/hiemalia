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

static const coord_t defaultMoveSpeed = 0.75;
static const coord_t minimumMoveSpeed = 0.25;
static const coord_t maximumMoveSpeed = 2;
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

bool GameMain::run(GameState& state, float interval) {
    static constexpr coord_t Y = 0.875;
    static const Rotation3D c_rot = Rotation3D(0, 0, 0);
    static const Rotation3D c_trot = Rotation3D(
        radians<coord_t>(-60), radians<coord_t>(30), radians<coord_t>(-30));
    static const ModelPoint c_scale = ModelPoint(0.25, 0.25, 0.25);
    if (!continue_) return false;
    if (gameOver_) {
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
        drawStatusBar();
    }

    paused_ = shouldBePaused_;
    if (!paused_ && state.controls.pause) pauseGame();
    if (!paused_) {
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
                    ModelPoint(p.x, p.y - 0.01171875, p.z - 0.03515625),
                    plr->rot, c_scale);
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
        auto& obj = w.objects;
        auto it = obj.begin();
        while (it != obj.end()) {
            if ((*it)->update(*world_, interval)) {
                (*it)->render(state.sbuf, r3d_);
                ++it;
            } else {
                it = obj.erase(it);
            }
        }
        if (playerAlive) {
            w.renderPlayer(state.sbuf, r3d_);
        } else if (!w.respawn()) {
            // game over
            gameOver_ = true;
            timer = 5;
            std::string s = "GAME OVER";
            font_.setScale(font_.scaleX() * 2, font_.scaleY() * 2);
            font_.renderTextLine(statusbar_, -font_.getTextWidth(s) / 2, 0,
                                 white, s);
        }
        state.sbuf.push(Splinter(SplinterType::EndClip, 0, 0));
    }
    state.sbuf.append(statusbar_);
    return true;
}

}  // namespace hiemalia
