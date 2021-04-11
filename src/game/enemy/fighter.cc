/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/fighter.cc: implementation of EnemyFighter

#include "game/enemy/fighter.hh"

#include "game/world.hh"
#include "math.hh"

namespace hiemalia {
static constexpr int patternCount = 1;

EnemyFighter::EnemyFighter(const Point3D& pos, int pattern)
    : EnemyObject(pos, 6.0f), pattern_(pattern % patternCount) {
    useGameModel(GameModel::EnemyFighter);
    rot = Orient3D::atPlayer;
}

bool EnemyFighter::doEnemyTick(GameWorld& w, float delta) {
    movePattern(w, delta);
    killPlayerOnContact(w);
    return !isOffScreen();
}

bool EnemyFighter::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    doExplode(w);
    if (killedByPlayer) addScore(w, 500);
    return true;
}

void EnemyFighter::movePattern(GameWorld& w, float dt) {
    switch (pattern_) {
        case 0:
            fireTime_ += dt * w.difficulty().getFireRateMultiplier();
            killPlayerOnContact(w);
            if (w.isPlayerAlive() &&
                pos.z - w.getPlayerPosition().z > getCollisionRadius()) {
                while (fireTime_ >= 1) {
                    fireBulletAtPlayer<EnemyBulletSimple>(
                        w, model().vertices[0], 0.625f, 0.125f, 1.0f);
                    fireTime_ -= 1;
                }
            }
            break;
    }
}

}  // namespace hiemalia