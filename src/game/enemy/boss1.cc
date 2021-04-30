/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/boss1.cc: implementation of EnemyBoss1

#include "game/enemy/boss1.hh"

#include "game/gamemsg.hh"
#include "game/world.hh"
#include "hiemalia.hh"

namespace hiemalia {
EnemyBoss1::EnemyBoss1(const Point3D& pos) : EnemyObject(pos, 150.0f) {
    useGameModel(GameModel::EnemyBoss1);
    rot = Orient3D::atPlayer;
    fireTime_ = random(std::uniform_real_distribution<float>(0, 1));
}

void EnemyBoss1::onSpawn(GameWorld& w) { speed_ = w.pushBoss({40}, 0.5); }

bool EnemyBoss1::doEnemyTick(GameWorld& w, float delta) {
    if (pos.z - getCollisionRadius() > stageSpawnDistance) return true;
    coord_t tx = w.getPlayerPosition().x * 0.5;
    coord_t ty = w.getPlayerPosition().y * 0.5;
    coord_t tz = w.getPlayerPosition().z + 2.5;
    Point3D op = pos;
    Point3D np = Point3D(tx, ty, pos.z);
    coord_t v = 0.0625 * delta;
    Point3D dp = np - op;
    if (dp.length() > v) dp = dp.normalize() * v;
    pos += dp;
    if (pos.z < tz)
        pos.z = std::min(tz, pos.z + delta * 0.75);
    else if (pos.z > tz)
        pos.z = std::max(tz, pos.z - delta * 0.75);
    fireTime_ +=
        delta * (wall_ ? 3.6f : 0.6f) * w.difficulty().getFireRateMultiplier();
    killPlayerOnContact(w);
    if (w.isPlayerAlive()) {
        if (wall_ > 0) {
            while (fireTime_ >= 1 && wall_ > 0) {
                sendMessage(AudioMessage::playSound(
                    SoundEffect::EnemyFire5, pos - w.getPlayerPosition()));
                if (wall_ == 11) {
                    sendMessage(AudioMessage::playSound(
                        SoundEffect::EnemyFire4, pos - w.getPlayerPosition()));
                    fireBulletAtPlayer<EnemyBullet>(w, model().vertices[0],
                                                    0.75f, 0.03125f, 0.03125f);
                }
                fireBullet<EnemyBulletScalable>(w, model().vertices[0],
                                                Point3D(0, 0, -10), 2.5f, 0.0f,
                                                0.5f);
                for (int x = 1; x <= 5; ++x) {
                    fireBullet<EnemyBulletScalable>(w, model().vertices[0],
                                                    Point3D(x, 0, -10), 2.5f,
                                                    0.0f, 0.5f);
                    fireBullet<EnemyBulletScalable>(w, model().vertices[0],
                                                    Point3D(-x, 0, -10), 2.5f,
                                                    0.0f, 0.5f);
                    fireBullet<EnemyBulletScalable>(w, model().vertices[0],
                                                    Point3D(0, -x, -10), 2.5f,
                                                    0.0f, 0.5f);
                    fireBullet<EnemyBulletScalable>(w, model().vertices[0],
                                                    Point3D(0, x, -10), 2.5f,
                                                    0.0f, 0.5f);
                }
                fireTime_ -= 1;
                --wall_;
            }
            if (wall_ == 0) fireTime_ = 0.2f;
        } else {
            while (fireTime_ >= 1) {
                sendMessage(AudioMessage::playSound(
                    SoundEffect::EnemyFire4, pos - w.getPlayerPosition()));
                fireBulletAtPlayer<EnemyBullet>(w, model().vertices[0], 0.75f,
                                                0.03125f, 0.03125f);
                fireTime_ -= 1;
                wall_ = 16;
            }
        }
    }
    return !isOffScreen();
}

bool EnemyBoss1::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    doExplodeBoss(w);
    sendMessage(AudioMessage::playSound(SoundEffect::ExplodeLarge,
                                        pos - w.getPlayerPosition()));
    sendMessage(GameMessage::shakeCamera(0.125));
    if (speed_ >= 0) w.popBoss(speed_);
    if (killedByPlayer) {
        addScore(w, 3000);
        w.onEnemyKilled(*this);
    }
    return true;
}

}  // namespace hiemalia
