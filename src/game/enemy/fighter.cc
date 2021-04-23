/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/fighter.cc: implementation of EnemyFighter

#include "game/enemy/fighter.hh"

#include "audio.hh"
#include "game/world.hh"
#include "hiemalia.hh"
#include "math.hh"

namespace hiemalia {
static constexpr int patternCount = 10;

EnemyFighter::EnemyFighter(const Point3D& pos, int pattern)
    : EnemyObject(pos, 6.0f), pattern_(pattern % patternCount) {
    useGameModel(GameModel::EnemyFighter);
    fireTime_ =
        getRandomPool().random(std::uniform_real_distribution<float>(0, 1));
    rot = Orient3D::atPlayer;
    canHitWalls(true);
}

static bool crossover(float t, float dt, float t1) {
    return t <= t1 && t + dt >= t1;
}

void EnemyFighter::movePattern(GameWorld& w, float dt) {
    switch (pattern_) {
        case 0:
            fireAtPlayer(w, dt, 1.0);
            break;
        case 1:
            if (pos.z < 2 && v_ <= 5) {
                coord_t z = 2 - pos.z;
                v_ += z;
                vel = Point3D(0, 0, z);
                fireAtPlayer(w, dt, 1.5);
            } else if (v_ > 5) {
                vel *= 0.75;
                fireAtPlayer(w, dt, 1.0);
            } else {
                fireAtPlayer(w, dt, 1.0);
            }
            break;
        case 2:
            vel.y = sin(v_ * numbers::TAU<coord_t>);
            fireAtPlayer(w, dt, 1.0);
            v_ = frac(v_ + 0.5 * dt);
            break;
        case 3:
            if (crossover(t_, dt, 0.5))
                vel = Point3D(0, 0, -1 / 16);
            else if (t_ < 0.5)
                vel = Point3D(1, 0, 0), fireAtPlayer(w, dt, 0.5);
            else
                fireAtPlayer(w, dt, 1.0);
            break;
        case 4:
            if (crossover(t_, dt, 0.375))
                vel = Point3D(0, 0, -1 / 16);
            else if (t_ < 0.375)
                vel = Point3D(1, 0, 0), fireAtPlayer(w, dt, 0.5);
            else
                fireAtPlayer(w, dt, 1.0);
            break;
        case 5:
            if (crossover(t_, dt, 0.25))
                vel = Point3D(0, 0, -1 / 16);
            else if (t_ < 0.25)
                vel = Point3D(1, 0, 0), fireAtPlayer(w, dt, 0.5);
            else
                fireAtPlayer(w, dt, 1.0);
            break;
        case 6:
            if (crossover(t_, dt, 0.5))
                vel = Point3D(0, 0, -1 / 16);
            else if (t_ < 0.5)
                vel = Point3D(-1, 0, 0), fireAtPlayer(w, dt, 0.5);
            else
                fireAtPlayer(w, dt, 1.0);
            break;
        case 7:
            if (crossover(t_, dt, 0.375))
                vel = Point3D(0, 0, -1 / 16);
            else if (t_ < 0.375)
                vel = Point3D(-1, 0, 0), fireAtPlayer(w, dt, 0.5);
            else
                fireAtPlayer(w, dt, 1.0);
            break;
        case 8:
            if (crossover(t_, dt, 0.25))
                vel = Point3D(0, 0, -1 / 16);
            else if (t_ < 0.25)
                vel = Point3D(-1, 0, 0), fireAtPlayer(w, dt, 0.5);
            else
                fireAtPlayer(w, dt, 1.0);
            break;
        case 9:
            fireAtPlayer(w, dt, 1.75);
            break;
    }
    t_ += dt;
}

bool EnemyFighter::doEnemyTick(GameWorld& w, float delta) {
    movePattern(w, delta);
    killPlayerOnContact(w);
    return !isOffScreen();
}

bool EnemyFighter::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    doExplode(w);
    sendMessage(AudioMessage::playSound(SoundEffect::ExplodeMedium));
    if (killedByPlayer) {
        addScore(w, 500);
        w.onEnemyKilled(*this);
    }
    return true;
}

void EnemyFighter::fireAtPlayer(GameWorld& w, float dt, coord_t fireMul) {
    fireTime_ += static_cast<float>(fireMul * dt *
                                    w.difficulty().getFireRateMultiplier());
    if (w.isPlayerAlive() &&
        pos.z - w.getPlayerPosition().z > getCollisionRadius()) {
        while (fireTime_ >= 1) {
            sendMessage(AudioMessage::playSound(SoundEffect::EnemyFire2,
                                                pos - w.getPlayerPosition()));
            fireBulletAtPlayer<EnemyBulletSimple>(w, model().vertices[0], 1.0f,
                                                  0.125f, 1.0f);
            fireTime_ -= 1;
        }
    }
}

}  // namespace hiemalia