/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/destroyer.cc: implementation of EnemyDestroyer

#include "game/enemy/destroyer.hh"

#include "audio.hh"
#include "game/world.hh"
#include "hiemalia.hh"
#include "math.hh"

namespace hiemalia {
static constexpr int patternCount = 3;

EnemyDestroyer::EnemyDestroyer(const Point3D& pos, int pattern)
    : EnemyObject(pos, 6.0f), pattern_(pattern % patternCount) {
    useGameModel(GameModel::EnemyDestroyer);
    rot = Orient3D::atPlayer;
}

void EnemyDestroyer::movePattern(GameWorld& w, float dt) {
    switch (pattern_) {
        case 0:
            fireAtPlayer(w, dt, 1.5);
            break;
        case 1:
            if (phase_ == 0) {
                vel = Point3D(0, 0, -w.getMoveSpeed());
                fireAtPlayer(w, dt, 0.5);
                if (pos.z - w.getPlayerPosition().z < 4) ++phase_;
            } else {
                vel = Point3D(0, 0, w.getMoveSpeed() * 0.5);
                fireAtPlayer(w, dt, 1.5);
            }
            break;
        case 2:
            if (phase_ == 0) {
                vel = Point3D(0, 0, 0) - pos;
                vel.z = 0;
                vel = vel.normalize() * 0.5;
                vel.z = w.getMoveSpeed() * 0.5;
                fireAtPlayer(w, dt, 0.5);
                if (abs(pos.x) < 0.625 && abs(pos.y) < 0.625) {
                    canHitWalls(true);
                    ++phase_;
                }
            } else {
                vel = Point3D(0, 0, w.getMoveSpeed() * 0.5);
                fireAtPlayer(w, dt, 1.5);
            }
            break;
    }
    t_ += dt;
}

bool EnemyDestroyer::doEnemyTick(GameWorld& w, float delta) {
    movePattern(w, delta);
    killPlayerOnContact(w);
    return !isOffScreen();
}

bool EnemyDestroyer::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    doExplode(w);
    sendMessage(AudioMessage::playSound(SoundEffect::ExplodeMedium));
    if (killedByPlayer) {
        addScore(w, 750);
        w.onEnemyKilled(*this);
    }
    return true;
}

void EnemyDestroyer::fireAtPlayer(GameWorld& w, float dt, coord_t fireMul) {
    fireTime_ += static_cast<float>(fireMul * dt *
                                    w.difficulty().getFireRateMultiplier());
    if (w.isPlayerAlive() &&
        pos.z - w.getPlayerPosition().z > getCollisionRadius()) {
        while (fireTime_ >= 1) {
            sendMessage(AudioMessage::playSound(SoundEffect::EnemyFire4,
                                                pos - w.getPlayerPosition()));
            fireBulletAtPlayer<EnemyBulletHoming>(w, model().vertices[0], 0.75f,
                                                  0.125f, 1.0f);
            fireTime_ -= 1;
        }
    }
}

}  // namespace hiemalia