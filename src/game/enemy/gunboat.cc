/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/gunboat.cc: implementation of EnemyGunboat

#include "game/enemy/gunboat.hh"

#include "game/world.hh"
#include "math.hh"
#include "random.hh"

namespace hiemalia {
EnemyGunboat::EnemyGunboat(const Point3D& pos) : EnemyObject(pos) {
    useGameModel(GameModel::EnemyGunboat);
    rot = Orient3D::atPlayer;
    fireTime_ = random(std::uniform_real_distribution<float>(0, 1));
    vel = Point3D(0, 0, -1 / 16);
}

bool EnemyGunboat::doEnemyTick(GameWorld& w, float delta) {
    rot.roll = sin(pos.z * 0.1) / 8;
    fireTime_ += delta * 0.8f * w.difficulty().getFireRateMultiplier();
    killPlayerOnContact(w);
    if (w.isPlayerAlive() &&
        pos.z - w.getPlayerPosition().z > getCollisionRadius()) {
        while (fireTime_ >= 1) {
            fireBulletAtPlayer<EnemyBulletSimple>(w, model().vertices[0],
                                                  0.625f, 0.125f, 0.0f);
            fireTime_ -= 1;
        }
    }
    return !isOffScreen();
}

bool EnemyGunboat::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    doExplode(w);
    if (killedByPlayer) {
        addScore(w, 200);
        w.onEnemyKilled(*this);
    }
    return true;
}

}  // namespace hiemalia
