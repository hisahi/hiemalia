/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/orbiter.cc: implementation of EnemyOrbiter

#include "game/enemy/orbiter.hh"

#include "audio.hh"
#include "game/world.hh"
#include "hiemalia.hh"
#include "math.hh"

namespace hiemalia {
static constexpr int patternCount = 3;

EnemyOrbiter::EnemyOrbiter(const Point3D& pos, int pattern)
    : EnemyObject(pos, 6.0f), pattern_(pattern % patternCount) {
    useGameModel(GameModel::EnemyOrbiter);
    rot = Orient3D::atPlayer;
    spread_ = getRandomPool().random(std::uniform_int_distribution(0, 3));
    canHitWalls(true);
    ox_ = pos.x;
    oy_ = pos.y;
}

void EnemyOrbiter::movePattern(GameWorld& w, float dt) {
    switch (pattern_) {
        case 0:
            fireAtPlayer(w, dt, 1.0);
            break;
        case 1:
            pos.x = ox_ + sin(t_ * 1) * (0.5 - 0.25 * cos(t_ * 0.37));
            pos.y = ox_ + cos(t_ * 1) * (0.5 - 0.25 * cos(t_ * 0.37));
            pos.z += w.getMoveSpeed() * 0.5 * dt;
            fireAtPlayer(w, dt, 1.5);
            break;
        case 2:
            pos.x = ox_ + sin(t_ * -1) * (0.5 - 0.25 * cos(t_ * 0.37));
            pos.y = ox_ + cos(t_ * -1) * (0.5 - 0.25 * cos(t_ * 0.37));
            pos.z += w.getMoveSpeed() * 0.5 * dt;
            fireAtPlayer(w, dt, 1.5);
            break;
    }
    t_ += dt;
}

bool EnemyOrbiter::doEnemyTick(GameWorld& w, float delta) {
    movePattern(w, delta);
    killPlayerOnContact(w);
    return !isOffScreen();
}

bool EnemyOrbiter::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    doExplode(w);
    sendMessage(AudioMessage::playSound(SoundEffect::ExplodeMedium));
    if (killedByPlayer) {
        addScore(w, 600);
        w.onEnemyKilled(*this);
    }
    return true;
}

void EnemyOrbiter::fireAtPlayer(GameWorld& w, float dt, coord_t fireMul) {
    fireTime_ += static_cast<float>(fireMul * dt *
                                    w.difficulty().getFireRateMultiplier());
    if (w.isPlayerAlive() &&
        pos.z - w.getPlayerPosition().z > getCollisionRadius()) {
        while (fireTime_ >= 1) {
            sendMessage(AudioMessage::playSound(SoundEffect::EnemyFire2,
                                                pos - w.getPlayerPosition()));
            bool doSpread = !spread_;
            spread_ = (spread_ + 1) % 4;
            fireBulletAtPlayer<EnemyBulletSimple>(w, model().vertices[0], 0.75f,
                                                  0.0625f, 1.0f);
            if (doSpread) {
                for (int i = 0; i < 4; ++i)
                    fireBulletAtPlayer<EnemyBulletSimple>(
                        w, model().vertices[0], 0.75f, 0.375f, 1.0f);
            }
            fireTime_ -= 1;
        }
    }
}

}  // namespace hiemalia