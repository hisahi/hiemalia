/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/pewpew.cc: implementation of EnemyPewpew

#include "game/enemy/pewpew.hh"

#include "audio.hh"
#include "game/world.hh"
#include "hiemalia.hh"
#include "math.hh"
#include "random.hh"

namespace hiemalia {
EnemyPewpew::EnemyPewpew(const Point3D& pos, const Orient3D& r)
    : EnemyObject(pos, 6.0f) {
    useGameModel(GameModel::EnemyPewpew);
    targetRot_ = rot = r + Orient3D::atPlayer;
    fireTime_ =
        getRandomPool().random(std::uniform_real_distribution<float>(0, 1));
}

void EnemyPewpew::aim(GameWorld& w, float delta) {
    Orient3D old = targetRot_;
    Point3D offset = w.getPlayerPosition() - pos;
    Orient3D newPredicted =
        Orient3D::toPolar(old.direction(offset.length()) +
                          Point3D(0, 0, delta * w.getMoveSpeed()));
    Orient3D newActual = Orient3D::toPolar(offset);
    float penalty = static_cast<float>(newActual.offBy(newPredicted)) * 1.0f;
    fireTime_ = std::max<float>(0, fireTime_ - penalty);
    targetRot_ = newActual;
}

bool EnemyPewpew::doEnemyTick(GameWorld& w, float delta) {
    if (!burst_ && rot != targetRot_) {
        rot = rot.tendTo(targetRot_, delta);
    }
    if (burst_ || rot == targetRot_)
        fireTime_ += delta * (burst_ ? 20.0f : 1.0f) *
                     w.difficulty().getFireRateMultiplier();
    if (!burst_) aim(w, delta);
    killPlayerOnContact(w);
    if (w.isPlayerAlive() &&
        pos.z - w.getPlayerPosition().z > getCollisionRadius()) {
        const float speed = 3.0f;
        Point3D dir = aimAtPlayer(w, speed, 1.0f);
        while (fireTime_ >= 1) {
            if (burst_)
                --burst_;
            else {
                burst_ = 10;
                sendMessage(AudioMessage::playSound(
                    SoundEffect::EnemyFire6, pos - w.getPlayerPosition()));
            }
            fireBullet<EnemyBulletLaser>(
                w, model().vertices[0] + dir * delta * frac(fireTime_), dir,
                speed, 0.0f);
            fireTime_ -= 1;
        }
    }
    return !isOffScreen();
}

bool EnemyPewpew::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
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
