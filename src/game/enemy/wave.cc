/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/wave.cc: implementation of EnemyWave

#include "game/enemy/wave.hh"

#include "audio.hh"
#include "game/world.hh"
#include "hiemalia.hh"
#include "math.hh"

namespace hiemalia {

EnemyWave::EnemyWave(const Point3D& pos, coord_t sx, coord_t sy, coord_t sz,
                     coord_t p, coord_t v)
    : EnemyObject(pos, 6.0f),
      off_(sx, sy, sz),
      v_(v * numbers::TAU<coord_t>),
      x_(p * numbers::TAU<coord_t>),
      pos_(pos) {
    useGameModel(GameModel::EnemyWave);
    rot = Orient3D::atPlayer;
    fireTime_ =
        getRandomPool().random(std::uniform_real_distribution<float>(0, 1));
}

bool EnemyWave::doEnemyTick(GameWorld& w, float delta) {
    fireTime_ += delta * 1.0f * w.difficulty().getFireRateMultiplier();
    pos = pos_ + off_ * sin(x_);
    x_ = wrapAngle(x_ + v_ * delta);
    rot.roll += 0.4 * delta;
    killPlayerOnContact(w);
    if (w.isPlayerAlive() &&
        pos.z - w.getPlayerPosition().z > getCollisionRadius()) {
        const Point3D& p = model().vertices[13];
        const Point3D& d = Point3D(0, 0, -1);
        while (fireTime_ >= 1) {
            sendMessage(AudioMessage::playSound(SoundEffect::EnemyFire2,
                                                pos - w.getPlayerPosition()));
            fireBullet<EnemyBulletSimple>(w, p, d, 1.0f, 0.0f);
            fireTime_ -= 1;
        }
    }
    return !isOffScreen();
}

void EnemyWave::onMove(const Point3D& newPos) {
    pos_ = newPos - off_ * sin(x_);
}

bool EnemyWave::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    doExplode(w);
    sendMessage(AudioMessage::playSound(SoundEffect::ExplodeMedium,
                                        pos - w.getPlayerPosition()));
    if (killedByPlayer) {
        addScore(w, 300);
        w.onEnemyKilled(*this);
    }
    return true;
}
}  // namespace hiemalia