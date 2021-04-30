/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/boss7.cc: implementation of EnemyBoss7

#include "game/enemy/boss7.hh"

#include "game/emissile.hh"
#include "game/gamemsg.hh"
#include "game/world.hh"
#include "hiemalia.hh"

namespace hiemalia {

EnemyBoss7::EnemyBoss7(const Point3D& pos)
    : EnemyObject(pos, 120.0f), lastTx_(0), lastTy_(0) {
    useGameModel(GameModel::EnemyBoss7);
    fireTime_ =
        getRandomPool().random(std::uniform_real_distribution<float>(0, 1));
    fireTime2_ =
        getRandomPool().random(std::uniform_real_distribution<float>(0, 1));
    rot = Orient3D::atPlayer;
    bModel_ = getGameModel(GameModel::EnemyBoss7B);
    cModel_ = getGameModel(GameModel::EnemyBoss7C);
}

bool EnemyBoss7::hitEnemy(GameWorld& w, float dmg,
                          const Point3D& pointOfContact) {
    return invul_ == 0;
}

void EnemyBoss7::onSpawn(GameWorld& w) { speed_ = w.pushBoss({59}, 0.333); }

void EnemyBoss7::render(SplinterBuffer& sbuf, Renderer3D& r3d) {
    EnemyObject::render(sbuf, r3d);
    if (phase_ < 2) r3d.renderModel(sbuf, pos, rot, scale, *bModel_.model);
    if (phase_ < 1) r3d.renderModel(sbuf, pos, rot, scale, *cModel_.model);
}

static float getFlipInterval() {
    float f =
        getRandomPool().random(std::uniform_real_distribution<float>(1, 3.5));
    return f * f;
}

bool EnemyBoss7::doEnemyTick(GameWorld& w, float delta) {
    if (pos.z - getCollisionRadius() > stageSpawnDistance) return true;
    if (invul_ > 0) invul_ = std::max<float>(0, invul_ - delta);
    killPlayerOnContact(w);
    coord_t tz = w.getPlayerPosition().z + 2.75;
    if (pos.z < tz)
        pos.z = std::min(tz, pos.z + delta * (pos.z <= 1.5 ? 2.0f : 1.0f));
    else if (pos.z > tz)
        pos.z = std::max(tz, pos.z - delta * 0.75);
    if (!w.isPlayerAlive()) return true;
    switch (phase_) {
        case 0: {
            if (pos.z > 3.5) return true;
            Point3D p = model().vertices[0];
            Point3D v =
                Point3D(0, 0, -1.5) * w.difficulty().getBulletSpeedMultiplier();
            constexpr coord_t f = 1.0 / 4;
            constexpr coord_t a = f / 2;
            fireTime_ = fireTime_ +
                        0.7f * delta * w.difficulty().getFireRateMultiplier();
            while (fireTime_ >= 1) {
                int txd = getRandomPool().random(
                    std::uniform_int_distribution(-1, 1));
                int tyd = getRandomPool().random(
                    std::uniform_int_distribution(-1, 1));
                if (getRandomPool().random(
                        std::uniform_int_distribution(0, 2)) == 0)
                    txd *= 2;
                if (getRandomPool().random(
                        std::uniform_int_distribution(0, 2)) == 0)
                    tyd *= 2;
                sendMessage(AudioMessage::playSound(
                    SoundEffect::EnemyFire1, pos - w.getPlayerPosition()));
                int tx = lastTx_;
                int ty = lastTy_;
                if (tx + txd < -3 || tx + txd > 3) txd = -txd;
                if (ty + tyd < -3 || ty + tyd > 3) tyd = -tyd;
                tx += txd;
                ty += tyd;
                for (int y = -4; y < 4; ++y) {
                    for (int x = -4; x < 4; ++x) {
                        if (x >= tx - 1 && x < tx + 1 && y >= ty - 1 &&
                            y < ty + 1)
                            continue;
                        Point3D t = Point3D(x * f + a, y * f + a, 2.25);
                        fireBullet<EnemyBulletSlideScalable>(
                            w, p, t - p, static_cast<float>(t.length() * 0.5),
                            0, t, v, 2.5);
                    }
                }
                lastTx_ = tx;
                lastTy_ = ty;
                fireTime_ -= 1;
                if (++burst_ >= 10) {
                    burst_ = 0;
                    fireTime_ -= 1.5f;
                }
            }
            x_ = wrapAngle(x_ + 4 * delta);
            break;
        }
        case 1: {
            if (invul_ > 0) return true;
            coord_t s = sin(a_);
            coord_t c = cos(a_);
            while (fireTime_ >= 1) {
                sendMessage(AudioMessage::playSound(
                    SoundEffect::EnemyFire5, pos - w.getPlayerPosition()));
                for (int x = -5; x <= 5; ++x) {
                    fireBullet<EnemyBulletScalable>(w, model().vertices[0],
                                                    Point3D(x * s, x * c, -8),
                                                    3.0f, 0.0f, 0.5f, 1);
                }
                fireTime_ -= 1;
            }
            while (fireTime2_ >= 1) {
                sendMessage(AudioMessage::playSound(
                    SoundEffect::EnemyFire4, pos - w.getPlayerPosition()));
                w.fireEnemyBullet<EnemyMissileHoming>(
                    pos, getBulletVelocity(w, aimAtPlayer(w, 0.5f, 0.5f), 0.5f,
                                           0.0f));
                fireTime2_ -= 1;
            }
            fireTime_ += 8.0f * delta;
            fireTime2_ +=
                delta * 0.25f * w.difficulty().getFireRateMultiplier();
            a_ += 0.625 * delta * dir_;
            if (flip_ <= 0) {
                sendMessage(AudioMessage::playSound(
                    SoundEffect::DirFlip, pos - w.getPlayerPosition()));
                flip_ = getFlipInterval();
                dir_ = -dir_;
            }
            flip_ -= delta;
            break;
        }
        case 2: {
            if (invul_ > 0) return true;
            auto& pool = getRandomPool();
            static std::uniform_real_distribution<coord_t> dist{-1, 1};
            while (fireTime_ >= 1) {
                sendMessage(AudioMessage::playSound(
                    SoundEffect::EnemyFire7, pos - w.getPlayerPosition()));
                fireBullet<EnemyBullet>(
                    w, model().vertices[0],
                    Point3D(pool.random(dist), pool.random(dist), -0.5), 0.5f,
                    0.0f);
                fireTime_ -= 1;
            }
            fireTime_ += delta * 10.0f * w.difficulty().getFireRateMultiplier();
            rot += Orient3D(1.0 / 7, 1.0 / 11, 1.0 / 13) * delta * 24;
            pos.x = sin(a_ + (0.5 + 0.5 * sin(b_))) * 0.5;
            pos.y = cos(a_ * (3 + 2 * cos(c_)) + (0.5 + 0.375 * sin(b_))) * 0.5;
            a_ = wrapAngle(a_ + 0.65 * delta);
            b_ = wrapAngle(b_ + 1.23 * delta);
            c_ = wrapAngle(c_ + 1.37 * delta);
            break;
        }
    }
    return true;
}

bool EnemyBoss7::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    if (phase_ == 0) {
        if (killedByPlayer) {
            addScore(w, 1000);
        }
        sendMessage(AudioMessage::playSound(SoundEffect::ExplodeMedium,
                                            pos - w.getPlayerPosition()));
        w.explodeEnemy(*this, *cModel_.model);
        ++phase_;
        invul_ = 2.5f;
        setHealth(50.0f);
        flip_ = getFlipInterval();
        return false;
    } else if (phase_ == 1) {
        if (killedByPlayer) {
            addScore(w, 1000);
        }
        sendMessage(AudioMessage::playSound(SoundEffect::ExplodeMedium,
                                            pos - w.getPlayerPosition()));
        w.explodeEnemy(*this, *bModel_.model);
        ++phase_;
        invul_ = 2.5f;
        setHealth(100.0f);
        a_ = 0;
        return false;
    }
    sendMessage(AudioMessage::playSound(SoundEffect::ExplodeLarge,
                                        pos - w.getPlayerPosition()));
    doExplodeBoss(w);
    sendMessage(GameMessage::shakeCamera(0.0625));
    if (speed_ >= 0) w.popBoss(speed_);
    if (killedByPlayer) {
        addScore(w, 3000);
        w.onEnemyKilled(*this);
    }
    return true;
}

}  // namespace hiemalia
