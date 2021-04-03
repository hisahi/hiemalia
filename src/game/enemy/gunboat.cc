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
EnemyGunboat::EnemyGunboat() {
    useGameModel(GameModel::EnemyGunboat);
    rot = Rotation3D::atPlayer;
    offset = random(std::uniform_real_distribution<float>(0, 1));
}

bool EnemyGunboat::doEnemyTick(GameWorld& w, float delta) {
    pos.z -= delta / 128;
    rot.roll = sin(pos.z * 0.1) / 8;
    offset += delta * 0.5f * w.difficulty().getFireRateMultiplier();
    killPlayerOnContact(w);
    if (w.isPlayerAlive() &&
        pos.z - w.getPlayerPosition().z > getCollisionRadius() * 0.5)
        while (offset >= 1) {
            fireBullet<EnemyBulletSimple>(w, model().vertices[0],
                                          aimAtPlayer(w), 0.625f, 0.125f);
            offset -= 1;
        }
    return !isOffScreen();
}

bool EnemyGunboat::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    doExplode(w);
    if (killedByPlayer) addScore(w, 200);
    return true;
}

}  // namespace hiemalia
