/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/boss4.cc: implementation of EnemyBoss4

#include "game/enemy/boss4.hh"

#include "game/gamemsg.hh"
#include "game/world.hh"
#include "hiemalia.hh"

namespace hiemalia {

EnemyBoss4::EnemyBoss4(const Point3D& pos, int i) : EnemyObject(pos, 60.0f) {
    useGameModel(GameModel::EnemyBoss4);
    fireTime_ =
        getRandomPool().random(std::uniform_real_distribution<float>(0, 1));
    coord_t f = i / 3.0;
    m_ += numbers::TAU<coord_t> * f;
    fireTime_ += static_cast<float>(f);
    slip_ = i;
}

void EnemyBoss4::onSpawn(GameWorld& w) {
    speed_ = w.pushBoss({19, 20, 21, 22, 23, 24, 25, 26}, 0.333);
}

bool EnemyBoss4::doEnemyTick(GameWorld& w, float delta) {
    m_ = wrapAngle(m_ + delta * 0.25 * numbers::PI<coord_t>);
    if (pos.z - getCollisionRadius() > stageSpawnDistance) return true;
    fireTime_ += delta * (w.getBossesAlive() == 1 ? 1.25f : 0.75f) *
                 w.difficulty().getFireRateMultiplier();
    Point3D target =
        Point3D(0.75 * sin(m_), 0.5 * cos(m_), 2.25 + w.getPlayerPosition().z);
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
            if (!((slip_ = slip_ + 1) % w.getBossesAlive())) {
                w.fireEnemyBullet<EnemyBulletSimple>(
                    p, getBulletVelocity(
                           w, pl - Point3D(spread * s, spread * c, 0), speed,
                           spew));
                w.fireEnemyBullet<EnemyBulletSimple>(
                    p, getBulletVelocity(
                           w, pl - Point3D(spread * c, spread * -s, 0), speed,
                           spew));
                w.fireEnemyBullet<EnemyBulletSimple>(
                    p, getBulletVelocity(
                           w, pl - Point3D(spread * -s, spread * -c, 0), speed,
                           spew));
                w.fireEnemyBullet<EnemyBulletSimple>(
                    p, getBulletVelocity(
                           w, pl - Point3D(spread * -c, spread * s, 0), speed,
                           spew));
            }
            fireTime_ -= 1;
        }
    }
    return true;
}

bool EnemyBoss4::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    doExplodeBoss(w);
    sendMessage(AudioMessage::playSound(SoundEffect::ExplodeLarge,
                                        pos - w.getPlayerPosition()));
    sendMessage(GameMessage::shakeCamera(0.0625));
    w.popBoss(speed_);
    if (killedByPlayer) {
        addScore(w, 1000);
        w.onEnemyKilled(*this);
    }
    return true;
}

void Boss4Script::doScript(GameWorld& w, bool instant) {
    if (!instant) {
        Point3D p = Point3D(0, 0, farObjectBackPlane);
        w.spawnEnemy<EnemyBoss4>(p, 0);
        w.spawnEnemy<EnemyBoss4>(p + Point3D(0, 0, 0.5), 1);
        w.spawnEnemy<EnemyBoss4>(p + Point3D(0, 0, 1), 2);
    }
}

}  // namespace hiemalia
