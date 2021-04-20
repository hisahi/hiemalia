/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/boss0.cc: implementation of EnemyBoss0

#include "game/enemy/boss0.hh"

#include "game/gamemsg.hh"
#include "game/world.hh"
#include "hiemalia.hh"

namespace hiemalia {

EnemyBoss0::EnemyBoss0(const Point3D& pos) : EnemyObject(pos, 75.0f) {
    useGameModel(GameModel::EnemyBoss0);
    fireTime_ =
        getRandomPool().random(std::uniform_real_distribution<float>(0, 1));
}

void EnemyBoss0::onSpawn(GameWorld& w) { speed_ = w.pushBoss({39}, 0.333); }

bool EnemyBoss0::doEnemyTick(GameWorld& w, float delta) {
    if (pos.z - getCollisionRadius() > stageSpawnDistance) return true;
    fireTime_ += delta * 0.8f * w.difficulty().getFireRateMultiplier();
    m_ = wrapAngle(m_ + delta * 0.5 * numbers::PI<coord_t>);
    Point3D target = Point3D(0.625 * sin(m_), 0.625 * cos(m_),
                             2.25 + w.getPlayerPosition().z);
    if (mv_ < 1)
        mv_ = std::min<coord_t>(1, mv_ + delta * (1 / numbers::PI<coord_t>));
    vel = (target - pos) * mv_;
    rot += Orient3D(1.0 / 67, 1.0 / 37, 1.0 / 17) * (delta * 4);
    killPlayerOnContact(w);
    if (w.isPlayerAlive() &&
        pos.z - w.getPlayerPosition().z > getCollisionRadius() * 0.5) {
        const float speed = 0.625f;
        const float spew = 0.0f;
        const float lead = 0.5f;
        const float spread = 0.375f;
        Point3D p(0, 0, 0.2);
        p += pos;
        Point3D pl = aimAtPlayer(w, speed, lead);
        coord_t s = sin(rot.roll), c = cos(rot.roll);
        while (fireTime_ >= 1) {
            sendMessage(AudioMessage::playSound(SoundEffect::EnemyFire2,
                                                pos - w.getPlayerPosition()));
            w.fireEnemyBullet<EnemyBulletSimple>(
                p, getBulletVelocity(w, pl, speed, spew));
            w.fireEnemyBullet<EnemyBulletSimple>(
                p, getBulletVelocity(w, pl - Point3D(spread * s, spread * c, 0),
                                     speed, spew));
            w.fireEnemyBullet<EnemyBulletSimple>(
                p,
                getBulletVelocity(w, pl - Point3D(spread * c, spread * -s, 0),
                                  speed, spew));
            w.fireEnemyBullet<EnemyBulletSimple>(
                p,
                getBulletVelocity(w, pl - Point3D(spread * -s, spread * -c, 0),
                                  speed, spew));
            w.fireEnemyBullet<EnemyBulletSimple>(
                p,
                getBulletVelocity(w, pl - Point3D(spread * -c, spread * s, 0),
                                  speed, spew));
            fireTime_ -= 1;
        }
    }
    return true;
}

bool EnemyBoss0::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
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
