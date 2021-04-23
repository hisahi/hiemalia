/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/gunboat2.cc: implementation of EnemyGunboat2

#include "game/enemy/gunboat2.hh"

#include "audio.hh"
#include "game/world.hh"
#include "hiemalia.hh"
#include "math.hh"
#include "random.hh"

namespace hiemalia {
EnemyGunboat2::EnemyGunboat2(const Point3D& pos) : EnemyObject(pos, 4.0f) {
    useGameModel(GameModel::EnemyGunboat2);
    rot = Orient3D::atPlayer;
    fireTime_ =
        getRandomPool().random(std::uniform_real_distribution<float>(0, 1));
    vel = Point3D(0, 0, -1 / 16);
}

bool EnemyGunboat2::doEnemyTick(GameWorld& w, float delta) {
    rot.roll = sin(pos.z * 0.1) / 8;
    fireTime_ += delta * 1.25f * w.difficulty().getFireRateMultiplier();
    killPlayerOnContact(w);
    if (w.isPlayerAlive() &&
        pos.z - w.getPlayerPosition().z > getCollisionRadius()) {
        while (fireTime_ >= 1) {
            sendMessage(AudioMessage::playSound(SoundEffect::EnemyFire2,
                                                pos - w.getPlayerPosition()));
            fireBulletAtPlayer<EnemyBulletSimple>(w, model().vertices[0], 0.75f,
                                                  0.125f, 1.0f);
            fireTime_ -= 1;
        }
    }
    return !isOffScreen();
}

bool EnemyGunboat2::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
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
