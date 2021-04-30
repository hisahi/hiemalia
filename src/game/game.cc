/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/game.cc: implementation of GameMain

#include "game/game.hh"

#include "assets.hh"
#include "game/enemy.hh"
#include "hiemalia.hh"
#include "logic.hh"
#include "math.hh"
#include "random.hh"

namespace hiemalia {

static const Color white{255, 255, 255, 255};
static const bool firstPerson = true;

GameMain::GameMain(const ConfigSectionPtr<GameConfig>& config_,
                   const std::shared_ptr<DemoFile>& demo)
    : world_(std::make_unique<GameWorld>(config_)),
      config_(config_),
      demo_(demo),
      timer(0),
      objectLateZ(farObjectBackPlane) {
    font_.setFont(getAssets().menuFont);
    ring_ = getGameModel(GameModel::Ring);
    halt_ = 0.25;
    if (demo_)
        world_->difficulty_ = GameDifficulty{GameDifficultyLevel::Normal};
}

static void playStageMusic(int stageNum) {
    switch (stageNum) {
        case 1:
            sendMessage(AudioMessage::playMusic(MusicTrack::Stage1));
            break;
        case 2:
            sendMessage(AudioMessage::playMusic(MusicTrack::Stage2));
            break;
        case 3:
            sendMessage(AudioMessage::playMusic(MusicTrack::Stage3));
            break;
        case 4:
            sendMessage(AudioMessage::playMusic(MusicTrack::Stage4));
            break;
    }
}

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
    font.drawTextLineCenter(sbuf, x, y, white, "LIVES*" + s);
}

static void drawStageCounter(SplinterBuffer& sbuf, RendererText& font,
                             coord_t x, coord_t y, unsigned stage,
                             unsigned cycle) {
    static const std::string sn = std::to_string(stageCount);
    std::string s = stage ? std::to_string(stage) : " ";
    std::string c = std::to_string(cycle);
    c.insert(c.begin(), 3 - c.size(), ' ');
    font.drawTextLineCenter(sbuf, x, y, white, c + " * " + s + "/" + sn);
}

static void drawCreditCounter(SplinterBuffer& sbuf, RendererText& font,
                              coord_t x, coord_t y, unsigned credits,
                              bool freePlay) {
    std::string c = std::to_string(credits);
    if (freePlay) {
        font.drawTextLineCenter(sbuf, x, y - 0.03125, Color{64, 255, 64, 192},
                                "FREE", 0.75);
        font.drawTextLineCenter(sbuf, x, y + 0.03125, Color{64, 255, 64, 192},
                                "PLAY", 0.75);
    } else {
        c.insert(c.begin(), 2 - c.size(), ' ');
        font.drawTextLineCenter(sbuf, x, y, white, "CREDIT*" + c);
    }
}

void GameMain::drawStatusBar() {
    statusbar_.clear();
    if (demo_) {
        Color c{128, 255, 64, 192};
        font_.drawTextLineCenter(statusbar_, 0, -0.9375, c, "DEMO");
        font_.drawTextLineCenter(statusbar_, 0, 0.9375, c, "DEMO");
        return;
    }
    if (world_->lives >= 0) {
        drawScore(statusbar_, font_, -0.5, -0.9375, world_->score);
        drawScore(statusbar_, font_, 0.5, -0.9375, world_->highScore);
        drawLives(statusbar_, font_, arcade_ ? -0.625 : -0.5, 0.9375,
                  world_->lives);
        drawStageCounter(statusbar_, font_, arcade_ ? 0.625 : 0.5, 0.9375,
                         world_->stageNum, world_->cycle);
    }
    if (arcade_)
        drawCreditCounter(statusbar_, font_, 0, 0.9375, world_->continues_,
                          freePlay_);
}

void GameMain::startNewStage() {
    world_->startNewStage();
    if (demo_) world_->resetStage(demo_->offset());
}

void GameMain::pauseGame() {
    if (!paused_) {
        if (demo_ || gameOver_) {
            doExitGame();
            return;
        }
        if (arcade_) return;
        shouldBePaused_ = true;
        paused_ = true;
        sendMessage(LogicMessage::pauseMenu());
        sendMessage(AudioMessage::pause());
        sendMessage(AudioMessage::playSound(SoundEffect::Pause));
    }
}

void GameMain::gotMessage(const GameMessage& msg) {
    switch (msg.type) {
        case GameMessageType::CreditAdded:
            if (continue_) continueResponse_ = 1;
            [[fallthrough]];
        case GameMessageType::UpdateStatus:
            drawStatusBar();
            break;
        case GameMessageType::PauseGame:
            pauseGame();
            break;
        case GameMessageType::ResumeGame:
            sendMessage(AudioMessage::resume());
            shouldBePaused_ = false;
            break;
        case GameMessageType::ContinueGame:
            break;
        case GameMessageType::StageComplete:
            doStageComplete();
            break;
        case GameMessageType::GameComplete:
            doGameComplete();
            break;
        case GameMessageType::ExitGame:
            doExitGame();
            break;
        case GameMessageType::ShakeCamera:
            cameraShake_ = msg.getFactor();
            shake1_ = Point3D{0, 0, 0};
            cameraShakeTime_ = 1;
            cameraShakeSpeed_ = pow(cameraShake_, 1.25);
            break;
        case GameMessageType::AddCredits:
            if (demo_) {
                sendMessage(HostMessage::mainMenuFromDemo());
                instaExit_ = true;
                return;
            }
            world_->addCredits(static_cast<int>(msg.getCredits()));
            break;
    }
}

void GameMain::doExitGame() {
    running_ = false;
    sendMessage(AudioMessage::stopMusic());
    sendMessage(AudioMessage::stopSounds());
}

void GameMain::gotMessage(const MenuMessage& msg) {
    if (continue_) {
        if (msg.type == MenuMessageType::MenuSelect)
            continueResponse_ = 1;
        else if (msg.type == MenuMessageType::MenuExit)
            continueResponse_ = -1;
    } else if (demo_ && msg.type == MenuMessageType::MenuSelect) {
        doExitGame();
    } else if (!paused_ && msg.type == MenuMessageType::MenuExit) {
        pauseGame();
    }
}

void GameMain::drawObject(GameState& state, float interval,
                          const ObjectPtr& obj) {
    if (obj->pos.z < objectLateZ) obj->render(state.sbuf, r3d_);
}

bool GameMain::updateObject(GameState& state, float interval,
                            const ObjectPtr& obj) {
    bool ok = obj->tick(*world_, interval);
    if (ok) drawObject(state, interval, obj);
    return ok;
}

void GameMain::doStageStart() {
    GameWorld& w = *world_;
    if (!demo_) {
        stageStartTimer = 5;
        sendMessage(AudioMessage::playMusic(MusicTrack::StageStart));
        textscreen_.clear();
        font_.drawTextLineCenter(textscreen_, 0, -0.25, white,
                                 "STAGE " + std::to_string(w.stageNum), 2);
    } else
        textscreen_.clear();
}

static bool timerCrosses(float timer, float interval, float threshold) {
    return timer > threshold && timer - interval <= threshold;
}

static bool timerCrossesP(float timer, float interval, float threshold) {
    return timer < threshold && timer + interval >= threshold;
}

static const Point3D c_scale = Point3D(0.25, 0.25, 0.25);
static const coord_t p_fpx = 0;
static const coord_t p_fpy = -0.017578125;
static const coord_t p_fpz = -0.052734375;

void GameMain::doStageStartTick(GameState& state, float interval) {
    GameWorld& w = *world_;
    static const coord_t backDistance = -0.25;
    if (stageStartTimer < 4) {
        float prog = unlerp(4.0f, stageStartTimer, 1.0f);
        if (prog >= 1) {
            prog = unlerp(1.0f, stageStartTimer, 0.0f);
            r3d_.setCamera(Point3D::sqerp(Point3D(0, 0, backDistance), prog,
                                          Point3D(p_fpx, p_fpy, p_fpz)),
                           w.getPlayer().rot, c_scale);
        } else {
            float radius = 1 - prog;
            float angle = prog * numbers::TAU<float>;
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
                Point3D(8 * r2 * s, -r2 * 4,
                        lerp<coord_t>(0, prog, backDistance) - 8 * r2 * c),
                Orient3D(-std::atan2(4 * r2 * s, 4 * r2 * c), r2 * 2, -radius),
                c_scale);
        }
        w.drawStage(state.sbuf, r3d_);
        w.renderPlayer(state.sbuf, r3d_, {0, 0, 0});
    }
    if (timerCrosses(stageStartTimer, interval, 1)) {
        font_.drawTextLineCenter(textscreen_, 0, 0.25, white, "START!", 2);
    }
    stageStartTimer -= interval;
    if (stageStartTimer <= 0) {
        textscreen_.clear();
        playStageMusic(w.stageNum);
        stageTime = 0;
    }
}

void GameMain::doStageComplete() {
    GameWorld& w = *world_;
    if (demo_) {
        doExitGame();
        return;
    }
    timer = 0;
    stageStartTimer = 0;
    stageComplete_ = true;
    objectLateZ = w.getObjectBackPlane();
    w.setNewSpeed(2, w.getMoveSpeed() / 4);
    textscreen_.clear();
    font_.drawTextLineCenter(textscreen_, 0, -0.5, white,
                             "STAGE " + std::to_string(w.stageNum) + " CLEAR",
                             1.5);
    dynamic_assert(w.isPlayerAlive() && w.getPlayer().playerInControl(),
                   "cannot complete stage when dead");
    sendMessage(AudioMessage::fadeOutMusic());
    bonus_ = 0;
    bonusIndex_ = 0;
}

void GameMain::doGameCompleteTick(GameState& state, float interval) {
    GameWorld& w = *world_;
    auto& plr = w.player;
    const Point3D& p = w.getPlayerPosition();
    Point3D cam = Point3D(p.x + p_fpx, p.y + p_fpy, p.z + p_fpz);
    r3d_.setCamera(cam, plr->rot, c_scale);
    w.go(interval);
    w.drawStage(state.sbuf, r3d_);
    plr->updateGameEnd(w, interval);
    w.renderPlayer(state.sbuf, r3d_, {0, 0, 0});
    timer -= interval;
    if (timer <= 0) doStageComplete();
}

void GameMain::doGameComplete() {
    // GameWorld& w = *world_;
    if (demo_) {
        doExitGame();
        return;
    }
    timer = 3;
    gameComplete_ = true;
}

static unsigned getTimeBonus(float time) {
    if (time > 900) return 0;
    return static_cast<unsigned>(pow(1800 - time * 2.0f, 1.5f) * 0.02618914f) *
           10;
}

static float getBonusY(int index) {
    return static_cast<float>(index) * 0.0625f;
}

void GameMain::doStageCompleteTick(GameState& state, float interval) {
    static const float timerEnd = 8;
    GameWorld& w = *world_;
    w.drawStage(state.sbuf, r3d_);
    w.renderPlayer(state.sbuf, r3d_, {0, 0, 0});
    w.moveForward(interval * w.getMoveSpeed());
    PlayerObject& p = w.getPlayer();
    if (p.rot.roll < 0)
        p.rot.roll = std::min<coord_t>(0, p.rot.roll + interval);
    else if (p.rot.roll > 0)
        p.rot.roll = std::max<coord_t>(0, p.rot.roll - interval);
    if (p.rot.pitch < 0)
        p.rot.pitch = std::min<coord_t>(0, p.rot.pitch + interval);
    else if (p.rot.pitch > 0)
        p.rot.pitch = std::max<coord_t>(0, p.rot.pitch - interval);
    coord_t roll = timer / timerEnd * 2;
    Orient3D rot =
        p.rot + (gameComplete_ ? Orient3D(0, 0, roll)
                               : Orient3D(0, -timer / (timerEnd * 4), 0));
    if (gameComplete_) {
        r3d_.setCamera(
            Point3D(p.pos.x + p_fpx, p.pos.y + p_fpy, p.pos.z + p_fpz - timer),
            rot, c_scale);
        roll *= 4;
        p.pos.x += roll * roll * roll * interval * 1.5;
    } else
        r3d_.setCamera(Point3D(p.pos.x + p_fpx,
                               p.pos.y + p_fpy - (timer * timer) / timerEnd,
                               p.pos.z + p_fpz - timer / 2),
                       rot, c_scale);
    if (timerCrossesP(timer, interval, 1)) {
        unsigned timeBonus = getTimeBonus(stageTime);
        font_.drawTextLineLeft(textscreen_, -0.75, getBonusY(bonusIndex_),
                               white, "TIME BONUS", 1);
        font_.drawTextLineRight(textscreen_, 0.75, getBonusY(bonusIndex_),
                                white, std::to_string(timeBonus), 1);
        ++bonusIndex_;
        bonus_ += timeBonus;
    }
    if (timerCrossesP(timer, interval, 1.5) && w.shouldGetNABonus()) {
        const unsigned naBonus_ = 20000;
        font_.drawTextLineLeft(textscreen_, -0.75, getBonusY(bonusIndex_),
                               white, "NONAGGRESSION BONUS", 1);
        font_.drawTextLineRight(textscreen_, 0.75, getBonusY(bonusIndex_),
                                white, std::to_string(naBonus_), 1);
        bonus_ += naBonus_;
        ++bonusIndex_;
    }
    if (timerCrossesP(timer, interval, 2) && w.stageNum == stageCount) {
        const unsigned completeBonus_ = 10000;
        font_.drawTextLineLeft(textscreen_, -0.75, getBonusY(bonusIndex_),
                               white, "COMPLETE BONUS", 1);
        font_.drawTextLineRight(textscreen_, 0.75, getBonusY(bonusIndex_),
                                white, std::to_string(completeBonus_), 1);
        bonus_ += completeBonus_;
        ++bonusIndex_;
    }
    if (timerCrossesP(timer, interval, 2.5)) {
        font_.drawTextLineLeft(textscreen_, -0.75, getBonusY(bonusIndex_),
                               white, "TOTAL BONUS", 1);
        font_.drawTextLineRight(textscreen_, 0.75, getBonusY(bonusIndex_),
                                white, std::to_string(bonus_), 1);
        ++bonusIndex_;
    }
    if (timerCrossesP(timer, interval, 3.5)) {
        ++bonusIndex_;
        font_.drawTextLineLeft(textscreen_, -0.75, getBonusY(bonusIndex_),
                               white, "OLD SCORE", 1);
        font_.drawTextLineRight(textscreen_, 0.75, getBonusY(bonusIndex_),
                                white, std::to_string(w.score), 1);
        ++bonusIndex_;
        w.addScore(bonus_);
        font_.drawTextLineLeft(textscreen_, -0.75, getBonusY(bonusIndex_),
                               white, "NEW SCORE", 1);
        font_.drawTextLineRight(textscreen_, 0.75, getBonusY(bonusIndex_),
                                white, std::to_string(w.score), 1);
        bonus_ = 0;
    }
    drawObjects(state, interval, w.objects);
    drawObjects(state, interval, w.enemies);
    drawObjects(state, interval, w.enemyBullets);
    drawObjects(state, interval, w.playerBullets);
    timer += interval;
    stageStartTimer = 0;
    if (timer >= timerEnd) {
        textscreen_.clear();
        stageComplete_ = false;
        gameComplete_ = false;
        w.nextStage = true;
    }
}

void GameMain::doGameOver() {
    // game over
    gameOver_ = true;
    timer = 5;
    sendMessage(AudioMessage::playMusic(MusicTrack::GameOver));
    stageComplete_ = false;
    font_.drawTextLineCenter(textscreen_, 0, 0, white, "GAME OVER", 2);
}

void GameMain::doContinuePrompt(int credits) {
    continue_ = true;
    timer = 11;
    continueResponse_ = 0;
    sendMessage(AudioMessage::playMusic(MusicTrack::Continue));
    stageComplete_ = false;
    blink_.clear();
    font_.drawTextLineCenter(textscreen_, 0, -0.5, Color{255, 255, 0, 255},
                             "CONTINUE?", 2);
    font_.drawTextLineCenter(blink_, 0, 0.125, white,
                             credits == 0 ? "INSERT COIN" : "PRESS FIRE BUTTON",
                             0.75);
    font_.drawTextLineCenter(blink_, 0, 0.1875, white, "TO CONTINUE PLAY",
                             0.75);
    font_.drawTextLineCenter(
        textscreen_, 0, 0.5,
        credits == 0 ? Color{255, 64, 64, 255} : Color{0, 255, 0, 192},
        "CREDIT  " + std::to_string(credits), 1);
    Orient3D o = Orient3D(0, -0.3, 0.3);
    r3d_.setCamera(Point3D(0, 0, 0) - o.direction(0.5), o,
                   Point3D(1, 1, 1) * 2);
}

void GameMain::doContinuePromptTick(GameState& state, float interval) {
    timer -= interval;
    coord_t r = ringRot_ * numbers::TAU<coord_t>;
    r3d_.renderModel(state.sbuf, Point3D(0, 0.0625, 0), Orient3D(r, 0, 0),
                     Point3D(1, 1, 1), *ring_.model);
    ringRot_ = frac(ringRot_ + interval / 32);
    state.sbuf.append(textscreen_);
    state.sbuf.append(statusbar_);
    font_.drawTextLineCenter(state.sbuf, 0, -0.125, Color{0, 255, 255, 224},
                             std::to_string(static_cast<int>(timer)), 1.5);
    if (frac(timer * 1.5) < 0.75) state.sbuf.append(blink_);
    if (state.controls.fire || continueResponse_ > 0) {
        if (world_->continues_ == 0) {
            continueResponse_ = 0;
            return;
        }
        sendMessage(AudioMessage::playSound(SoundEffect::MenuSelect));
        textscreen_.clear();
        blink_.clear();
        world_->spendContinue();
        continue_ = false;
        halt_ = 0.25f;
    } else if (continueResponse_ < 0 || timer < 0) {
        textscreen_.clear();
        blink_.clear();
        sendMessage(AudioMessage::stopMusic());
        continue_ = false;
        doGameOver();
    }
}

void GameMain::doInit(GameState& state) {
    GameWorld& w = *world_;
    w.highScore = state.highScores.getTopScore();
    arcade_ = state.arcade;
    freePlay_ = config_->arcadeFreePlay;
    if (!arcade_) w.continues_ = config_->maxContinues;
    drawStatusBar();
    init_ = true;
    if (demo_) world_->stageNum = demo_->stage() - 1;
}

bool GameMain::run(GameState& state, float interval) {
    static constexpr coord_t Y = 0.875;
    static const Orient3D c_rot = Orient3D(0, 0, 0);
    static const Orient3D c_trot = Orient3D(
        radians<coord_t>(-15), radians<coord_t>(30), radians<coord_t>(0));
    if (!init_) doInit(state);
    if (instaExit_) {
        dynamic_assert(demo_ != nullptr, "?");
        return false;
    }
    GameWorld& w = *world_;
    if (gameOver_) {
        state.sbuf.append(textscreen_);
        state.sbuf.append(statusbar_);
        timer -= interval;
        if (timer > 0) return true;
        doExitGame();
        gameOver_ = false;
    }
    if (!running_) {
        if (demo_) {
            sendMessage(HostMessage::mainMenuFromDemo());
            return false;
        }
        int rank = state.highScores.getHighscoreRank(w.score);
        if (rank >= 0) {
            sendMessage(
                HostMessage::gotHighScore({w.score, w.cycle, w.stageNum}));
        } else {
            sendMessage(HostMessage::mainMenu());
        }
        return false;
    }
    if (continue_) {
        doContinuePromptTick(state, interval);
        return true;
    }
    if (halt_ > 0) {
        state.sbuf.append(statusbar_);
        halt_ -= interval;
        return true;
    }

    if (w.nextStage) {
        w.nextStage = false;
        startNewStage();
        doStageStart();
        drawStatusBar();
    }

    paused_ = shouldBePaused_;
    if (!paused_ && !arcade_ && state.controls.pause) pauseGame();
    if (paused_) {
        // I am paused
        state.sbuf.append(statusbar_);
        return true;
    }

    if (stageStartTimer > 0) {
        state.sbuf.push(Splinter(SplinterType::BeginClipCenter, -Y, +Y));
        doStageStartTick(state, interval);
        state.sbuf.push(Splinter(SplinterType::EndClip, 0, 0));
    } else if (stageComplete_) {
        state.sbuf.push(Splinter(SplinterType::BeginClipCenter, -Y, +Y));
        doStageCompleteTick(state, interval);
        state.sbuf.push(Splinter(SplinterType::EndClip, 0, 0));
    } else if (gameComplete_) {
        state.sbuf.push(Splinter(SplinterType::BeginClipCenter, -Y, +Y));
        doGameCompleteTick(state, interval);
        state.sbuf.push(Splinter(SplinterType::EndClip, 0, 0));
    } else {
        state.sbuf.push(Splinter(SplinterType::BeginClipCenter, -Y, +Y));
        Orient3D envRot{0, 0, 0};
        if (demo_) {
            if (!demo_->runDemo(interval) || state.controls.fire ||
                state.controls.pause) {
                doExitGame();
                sendMessage(HostMessage::mainMenuFromDemo());
                return false;
            }
        }
        const ControlState& controls =
            demo_ ? demo_->getInputs() : state.controls;
        w.updateMoveSpeedInput(controls, interval);
        if (w.isPlayerAlive()) {
            w.player->updateInput(controls);
            w.go(interval);
        }
        bool playerAlive = w.runPlayer(interval);
        const Point3D& p = w.getPlayerPosition();
        stageTime += interval;
        if (w.isPlayerAlive()) {
            auto& plr = w.player;
            envRot = w.getSectionRotation();
            if (firstPerson) {  // first person view
                Point3D cam = Point3D(p.x + p_fpx, p.y + p_fpy, p.z + p_fpz);
                if (cameraShake_) {
                    coord_t s = cameraShake_ / 32;
                    while (cameraShakeTime_ >= 1) {
                        shake0_ = shake1_;
                        shake1_ = randomUnitVector();
                        cameraShakeTime_ -= 1;
                    }
                    cameraShakeTime_ += interval * 32;
                    cam +=
                        s * Point3D::lerp(shake0_, cameraShakeTime_, shake1_);
                    cameraShake_ = std::max<coord_t>(
                        0, cameraShake_ - interval * cameraShakeSpeed_);
                }
                r3d_.setCamera(cam, plr->rot + envRot, c_scale);
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
                r3d_.setCamera(Point3D(p.x, p.y, -0.25 + w.progress_f), c_rot,
                               c_scale);
        } else {
            r3d_.setCamera(Point3D(p.x + 0.125, p.y - 0.5, p.z - 0.5), c_trot,
                           c_scale);
            cameraShake_ = 0;
        }
        w.drawStage(state.sbuf, r3d_);
        objectLateZ = w.getObjectBackPlane();
        processObjects(state, interval, w.objects);
        processObjects(state, interval, w.enemies);
        objectLateZ = farObjectBackPlane;
        processObjects(state, interval, w.enemyBullets);
        processObjects(state, interval, w.playerBullets);
        if (playerAlive) {
            w.renderPlayer(state.sbuf, r3d_, envRot);
        } else if (demo_) {
            doExitGame();
        } else if (w.respawn()) {
            playStageMusic(w.stageNum);
        } else if (w.continuesRemaining() > 0 || (arcade_ && !freePlay_)) {
            doContinuePrompt(w.continuesRemaining());
        } else {
            doGameOver();
        }
        state.sbuf.push(Splinter(SplinterType::EndClip, 0, 0));
    }
    state.sbuf.append(textscreen_);
    state.sbuf.append(statusbar_);
    return true;
}

}  // namespace hiemalia
