/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/launcher.cc: implementation of EnemyLauncher

#include "game/enemy/launcher.hh"

#include "audio.hh"
#include "game/world.hh"
#include "hiemalia.hh"
#include "math.hh"
#include "random.hh"

namespace hiemalia {
EnemyLauncher::EnemyLauncher(const Point3D& pos, const Orient3D& r)
    : EnemyObject(pos, 6.0f) {
    useGameModel(GameModel::EnemyLauncher);
    rot = r;
    fireTime_ =
        getRandomPool().random(std::uniform_real_distribution<float>(0, 1));
}

bool EnemyLauncher::doEnemyTick(GameWorld& w, float delta) {
    fireTime_ += delta * 1.0f * w.difficulty().getFireRateMultiplier();
    killPlayerOnContact(w);
    if (w.isPlayerAlive() &&
        pos.z - w.getPlayerPosition().z > getCollisionRadius()) {
        while (fireTime_ >= 1) {
            sendMessage(AudioMessage::playSound(SoundEffect::EnemyFire2,
                                                pos - w.getPlayerPosition()));
            fireBulletAtPlayer<EnemyBulletHoming>(w, model().vertices[0], 0.75f,
                                                  0.25f, 0.0f);
            fireTime_ -= 1;
        }
    }
    return !isOffScreen();
}

bool EnemyLauncher::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    doExplode(w);
    sendMessage(AudioMessage::playSound(SoundEffect::ExplodeSmall,
                                        pos - w.getPlayerPosition()));
    if (killedByPlayer) {
        addScore(w, 300);
        w.onEnemyKilled(*this);
    }
    return true;
}

}  // namespace hiemalia
