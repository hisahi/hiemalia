/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/blocker.cc: implementation of EnemyBlocker

#include "game/enemy/blocker.hh"

#include "audio.hh"
#include "game/obstacle.hh"
#include "game/world.hh"
#include "hiemalia.hh"
#include "math.hh"
#include "random.hh"

namespace hiemalia {
static const coord_t dist = 1.25;

EnemyBlocker::EnemyBlocker(const Point3D& pos) : EnemyObject(pos, 12.0f) {
    useGameModel(GameModel::EnemyBlocker);
    rot = Orient3D::atPlayer;
    fireTime_ =
        getRandomPool().random(std::uniform_real_distribution<float>(0.75f, 1));
    canHitWalls(true);
}

bool EnemyBlocker::doEnemyTick(GameWorld& w, float delta) {
    fireTime_ += delta * 1.0f * w.difficulty().getFireRateMultiplier();
    killPlayerOnContact(w);
    if (w.isPlayerAlive() &&
        pos.z - w.getPlayerPosition().z > getCollisionRadius()) {
        Point3D dir = w.getPlayerPosition() - pos;
        dir.z += dir.length() * w.getMoveSpeed() * 0.25 + dist;
        while (fireTime_ >= 1) {
            sendMessage(AudioMessage::playSound(SoundEffect::EnemyFire4,
                                                pos - w.getPlayerPosition()));
            fireBullet<EnemyBulletBlocker>(w, model().vertices[0], dir, 1.0f,
                                           0.125f);
            fireTime_ -= 1;
        }
    }
    pos.z += delta * w.getMoveSpeed() / 2;
    return !isOffScreen();
}

bool EnemyBlocker::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    doExplode(w);
    sendMessage(AudioMessage::playSound(SoundEffect::ExplodeSmall,
                                        pos - w.getPlayerPosition()));
    if (killedByPlayer) {
        addScore(w, 700);
        w.onEnemyKilled(*this);
    }
    return true;
}

EnemyBulletBlocker::EnemyBulletBlocker(const Point3D& pos, const Point3D& v)
    : EnemyBulletSimple(pos, v) {
    useGameModel(GameModel::BulletEnemyBlocker);
}

bool EnemyBulletBlocker::doBulletTick(GameWorld& w, float delta) {
    if (pos.z <= dist) {
        sendMessage(AudioMessage::playSound(SoundEffect::BlockerPlace,
                                            pos - w.getPlayerPosition()));
        w.spawn<DestroyableObstacle>(pos, Orient3D(0, 0, 0),
                                     GameModel::ObstacleBlocker, 2.0f);
        return false;
    }
    return EnemyBulletSimple::doBulletTick(w, delta);
}

}  // namespace hiemalia
