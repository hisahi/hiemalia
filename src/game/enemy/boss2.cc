/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/boss2.cc: implementation of EnemyBoss2

#include "game/enemy/boss2.hh"

#include "game/emissile.hh"
#include "game/gamemsg.hh"
#include "game/world.hh"
#include "hiemalia.hh"

namespace hiemalia {

EnemyBoss2::EnemyBoss2(const Point3D& pos) : EnemyObject(pos, 75.0f) {
    useGameModel(GameModel::EnemyBoss2);
    fireTime_ =
        getRandomPool().random(std::uniform_real_distribution<float>(0, 1));
}

void EnemyBoss2::onSpawn(GameWorld& w) {
    speed_ = w.pushBoss(
        {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17}, 0.333);
}

bool EnemyBoss2::doEnemyTick(GameWorld& w, float delta) {
    if (pos.z - getCollisionRadius() > stageSpawnDistance) return true;
    fireTime_ += delta * 1.1f * w.difficulty().getFireRateMultiplier();
    coord_t tx = w.getPlayerPosition().x * 0.5;
    coord_t ty = w.getPlayerPosition().y * 0.5;
    coord_t tz = w.getPlayerPosition().z + 2.75;
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
    killPlayerOnContact(w);
    if (w.isPlayerAlive() &&
        pos.z - w.getPlayerPosition().z > getCollisionRadius() * 0.5) {
        const float speed = 0.75f;
        const float spew = 0.0f;
        const float lead = 0.375f;
        const float spread = 0.125f;
        Point3D p(0, 0, 0.2);
        p += pos;
        Point3D pl = aimAtPlayer(w, speed, lead);
        coord_t v_ = 0;
        coord_t s = sin(v_ * numbers::PI<coord_t>),
                c = cos(v_ * numbers::PI<coord_t>);
        while (fireTime_ >= 1) {
            sendMessage(AudioMessage::playSound(SoundEffect::EnemyFire2,
                                                pos - w.getPlayerPosition()));
            w.fireEnemyBullet<EnemyMissileHoming>(
                p, getBulletVelocity(w, pl, 0.5f, spew));
            w.fireEnemyBullet<EnemyBulletSimple>(
                p + Point3D(0.5 * s, 0.5 * c, 0),
                getBulletVelocity(w, pl + Point3D(spread * s, spread * c, 0),
                                  speed, spew));
            w.fireEnemyBullet<EnemyBulletSimple>(
                p + Point3D(0.5 * c, 0.5 * -s, 0),
                getBulletVelocity(w, pl + Point3D(spread * c, spread * -s, 0),
                                  speed, spew));
            w.fireEnemyBullet<EnemyBulletSimple>(
                p + Point3D(0.5 * -s, 0.5 * -c, 0),
                getBulletVelocity(w, pl + Point3D(spread * -s, spread * -c, 0),
                                  speed, spew));
            w.fireEnemyBullet<EnemyBulletSimple>(
                p + Point3D(0.5 * -c, 0.5 * s, 0),
                getBulletVelocity(w, pl + Point3D(spread * -c, spread * s, 0),
                                  speed, spew));
            fireTime_ -= 1;
        }
    }
    return true;
}

bool EnemyBoss2::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    doExplodeBoss(w);
    sendMessage(AudioMessage::playSound(SoundEffect::ExplodeLarge,
                                        pos - w.getPlayerPosition()));
    sendMessage(GameMessage::shakeCamera(0.0625));
    if (speed_ >= 0) w.popBoss(speed_);
    if (killedByPlayer) {
        addScore(w, 2500);
        w.onEnemyKilled(*this);
    }
    return true;
}

}  // namespace hiemalia
