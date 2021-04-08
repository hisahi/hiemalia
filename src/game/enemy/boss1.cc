/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/boss1.cc: implementation of EnemyBoss1

#include "game/enemy/boss1.hh"

#include "game/world.hh"

namespace hiemalia {
EnemyBoss1::EnemyBoss1(const Point3D& pos) : EnemyObject(pos, 50.0f) {
    useGameModel(GameModel::EnemyBoss1);
    fireTime_ = random(std::uniform_real_distribution<float>(0, 1));
}

void EnemyBoss1::onSpawn(GameWorld& w) { speed_ = w.pushBoss(); }

bool EnemyBoss1::doEnemyTick(GameWorld& w, float delta) {
    if (pos.z - getCollisionRadius() > stageSpawnDistance) return true;
    fireTime_ += delta * 0.8f * w.difficulty().getFireRateMultiplier();
    killPlayerOnContact(w);
    if (w.isPlayerAlive() &&
        pos.z - w.getPlayerPosition().z > getCollisionRadius() * 0.5) {
        while (fireTime_ >= 1) {
            fireBulletAtPlayer<EnemyBulletSimple>(w, model().vertices[0],
                                                  0.625f, 0.125f, 0.0f);
            fireTime_ -= 1;
        }
    }
    return !isOffScreen();
}

bool EnemyBoss1::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    doExplodeBoss(w);
    if (speed_ >= 0) w.popBoss(speed_);
    if (killedByPlayer) addScore(w, 2500);
    return true;
}

}  // namespace hiemalia
