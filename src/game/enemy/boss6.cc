/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/boss6.cc: implementation of EnemyBoss6

#include "game/enemy/boss6.hh"

#include "game/emissile.hh"
#include "game/gamemsg.hh"
#include "game/world.hh"
#include "hiemalia.hh"

namespace hiemalia {

static const coord_t width = 0.375;
static const coord_t height = 0.375;

EnemyBoss6::EnemyBoss6(const Point3D& pos) : EnemyObject(pos, 60.0f) {
    useGameModel(GameModel::EnemyBoss6);
    fireTime_ =
        getRandomPool().random(std::uniform_real_distribution<float>(0, 1));
    rot = Orient3D::atPlayer;
    bModel_ = getGameModel(GameModel::EnemyBoss6B);
}

bool EnemyBoss6::hitEnemy(GameWorld& w, float dmg,
                          const Point3D& pointOfContact) {
    return invul_ == 0 && (phase_ == 0 || rstate_ != 0);
}

void EnemyBoss6::onSpawn(GameWorld& w) { speed_ = w.pushBoss({59}, 0.5); }

void EnemyBoss6::render(SplinterBuffer& sbuf, Renderer3D& r3d) {
    EnemyObject::render(sbuf, r3d);
    if (phase_ == 0) r3d.renderModel(sbuf, pos, rot, scale, *bModel_.model);
}

bool EnemyBoss6::doEnemyTick(GameWorld& w, float delta) {
    if (pos.z - getCollisionRadius() > stageSpawnDistance) return true;
    if (invul_ > 0) invul_ = std::max<float>(0, invul_ - delta);
    killPlayerOnContact(w);
    coord_t tz = w.getPlayerPosition().z + 2.75;
    if (pos.z < tz)
        pos.z = std::min(tz, pos.z + delta * (pos.z <= 2 ? 3 - pos.z : 1));
    else if (pos.z > tz)
        pos.z = std::max(tz, pos.z - delta * 0.75);
    if (!w.isPlayerAlive()) return true;
    switch (phase_) {
        case 0: {
            coord_t tx = w.getPlayerPosition().x * (1 - width / 2);
            coord_t ty = w.getPlayerPosition().y * (1 - width / 2);
            const coord_t v = 0.333 * delta;
            Point3D p = model().vertices[0];
            Point3D op = pos;
            Point3D pv = Point3D(0, 0, -1.25) *
                         w.difficulty().getBulletSpeedMultiplier();
            Point3D np = Point3D(tx, ty, pos.z);
            Point3D dp = np - op;
            if (dp.length() > v) dp = dp.normalize() * v;
            pos += dp;

            while (fireTime_ >= 1) {
                sendMessage(AudioMessage::playSound(
                    SoundEffect::EnemyFire2, pos - w.getPlayerPosition()));
                for (int x = -2; x <= 2; ++x) {
                    for (int y = -2; y <= 2; ++y) {
                        if (abs(x) + abs(y) >= 4) continue;
                        Point3D t =
                            Point3D(tx + 0.1875 * x, ty + 0.1875 * y, 2.25);
                        fireBullet<EnemyBulletSlideScalable>(
                            w, p, t - p, static_cast<float>(t.length() * 0.5),
                            0, t, pv, 0.75);
                    }
                }
                fireTime_ -= 1.0f;
                x_ = 0;
            }
            fireTime_ += static_cast<float>(
                delta * 0.75 * w.difficulty().getFireRateMultiplier() -
                dp.length());
            x_ += dp.length() * 2.5;
            if (x_ >= 1) {
                sendMessage(AudioMessage::playSound(
                    SoundEffect::EnemyFire4, pos - w.getPlayerPosition()));
                w.fireEnemyBullet<EnemyMissileHoming>(
                    pos, getBulletVelocity(w, aimAtPlayer(w, 0.5f, 0.5f), 0.5f,
                                           0.0f));
                x_ = frac(x_);
            }
            break;
        }
        case 1:
            if (invul_ > 0) return true;
            switch (rstate_) {
                case 0: {
                    auto r = w.getMoveRegionForZ(pos.z);
                    auto ppos = w.getPlayerPosition();
                    coord_t tx =
                        clamp(r.x0 + width / 2, ppos.x, r.x1 - width / 2);
                    coord_t ty =
                        clamp(r.y0 + height / 2, ppos.y, r.y1 - height / 2);
                    Point3D op = pos;
                    Point3D np = Point3D(tx, ty, pos.z);
                    coord_t v = 0.4 * delta;
                    Point3D dp = np - op;
                    if (dp.length() > v) dp = dp.normalize() * v;

                    pos += dp;

                    fireTime_ += static_cast<float>(
                        delta * (w.difficulty().getFireRateMultiplier() +
                                 dp.length() * 0.5));
                    while (fireTime_ >= 1) {
                        sendMessage(AudioMessage::playSound(
                            SoundEffect::EnemyFire4,
                            pos - w.getPlayerPosition()));
                        w.fireEnemyBullet<EnemyMissileHoming>(
                            pos, getBulletVelocity(w, Point3D(0, 0, 0), 0.25f,
                                                   1.0f));
                        fireTime_ -= 1;
                    }

                    float_ += 4.0f * delta;
                    float_ = std::max(
                        0.0f, float_ - static_cast<float>(delta *
                                                          (op - pos).length()));

                    if (float_ >= 1 && abs(pos.x - tx) < 0.1 &&
                        abs(pos.y - ty) < 0.1) {
                        dist_ = pos.z - ppos.z + 1;
                        remaining_ = dist_;
                        rstate_ = 1;
                    }
                    break;
                }
                case 1: {
                    coord_t speed =
                        5 * delta * w.difficulty().getEnemySpeedMultiplier();
                    pos.z -= std::min(remaining_, speed);
                    remaining_ -= speed;
                    if (remaining_ < 0) {
                        remaining_ = dist_;
                        rstate_ = 2;
                    }
                    break;
                }
                case 2: {
                    coord_t speed =
                        2 * delta * w.difficulty().getEnemySpeedMultiplier();
                    pos.z += std::min(remaining_, speed);
                    remaining_ -= speed;
                    if (remaining_ < 0) {
                        remaining_ = dist_;
                        rstate_ = 0;
                    }
                    break;
                }
            }
            break;
    }
    return true;
}

bool EnemyBoss6::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    if (phase_ == 0) {
        if (killedByPlayer) {
            addScore(w, 1000);
        }
        sendMessage(AudioMessage::playSound(SoundEffect::ExplodeMedium,
                                            pos - w.getPlayerPosition()));
        w.explodeEnemy(*this, *bModel_.model);
        ++phase_;
        invul_ = 2.5f;
        setHealth(50.0f);
        return false;
    }
    sendMessage(AudioMessage::playSound(SoundEffect::ExplodeLarge,
                                        pos - w.getPlayerPosition()));
    doExplodeBoss(w);
    sendMessage(GameMessage::shakeCamera(0.0625));
    if (speed_ >= 0) w.popBoss(speed_);
    if (killedByPlayer) {
        addScore(w, 2000);
        w.onEnemyKilled(*this);
    }
    return true;
}

}  // namespace hiemalia
