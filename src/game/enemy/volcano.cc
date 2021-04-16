/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/volcano.cc: implementation of EnemyVolcano

#include "game/enemy/volcano.hh"

#include "audio.hh"
#include "game/world.hh"
#include "hiemalia.hh"

namespace hiemalia {
EnemyVolcano::EnemyVolcano(const Point3D& pos, const Orient3D& r,
                           const Orient3D& rv)
    : EnemyObject(pos, 5.0f), rotvel_(rv) {
    useGameModel(GameModel::EnemyVolcano);
    rot = r + Orient3D::atPlayer;
    fireTime_ = random(std::uniform_real_distribution<float>(0, 1));
}

bool EnemyVolcano::doEnemyTick(GameWorld& w, float delta) {
    fireTime_ += delta * 0.5f * w.difficulty().getFireRateMultiplier();
    rot += rotvel_ * delta;
    killPlayerOnContact(w);
    if (w.isPlayerAlive() &&
        pos.z - w.getPlayerPosition().z > getCollisionRadius() * 0.5) {
        const Point3D& p = Point3D(0, 0, 0.5 * 0.25);
        const Point3D& d = rot.rotate(p, 1.0);
        static const coord_t spread = 0.25;
        while (fireTime_ >= 1) {
            sendMessage(AudioMessage::playSound(SoundEffect::EnemyFire1,
                                                pos - w.getPlayerPosition()));
            fireBullet<EnemyBulletSimple>(w, p, d, 0.5f, 0.0f);
            fireBullet<EnemyBulletSimple>(w, p, d + Point3D(spread, spread, 0),
                                          0.5f, 0.0f);
            fireBullet<EnemyBulletSimple>(w, p, d + Point3D(spread, -spread, 0),
                                          0.5f, 0.0f);
            fireBullet<EnemyBulletSimple>(w, p, d + Point3D(-spread, spread, 0),
                                          0.5f, 0.0f);
            fireBullet<EnemyBulletSimple>(
                w, p, d + Point3D(-spread, -spread, 0), 0.5f, 0.0f);
            fireTime_ -= 1;
        }
    }
    return !isOffScreen();
}

bool EnemyVolcano::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    doExplode(w);
    sendMessage(AudioMessage::playSound(SoundEffect::ExplodeSmall,
                                        pos - w.getPlayerPosition()));
    if (killedByPlayer) {
        addScore(w, 250);
        w.onEnemyKilled(*this);
    }
    return true;
}

}  // namespace hiemalia
