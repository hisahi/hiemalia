/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/pod.cc: implementation of EnemyPod

#include "game/enemy/pod.hh"

#include "audio.hh"
#include "game/world.hh"
#include "hiemalia.hh"
#include "math.hh"
#include "random.hh"

namespace hiemalia {
EnemyPod::EnemyPod(const Point3D& pos) : EnemyObject(pos, 6.0f) {
    useGameModel(GameModel::EnemyPod);
    auto& pool = getRandomPool();
    rot.yaw = pool.random(
        std::uniform_real_distribution<coord_t>(0, numbers::TAU<coord_t>));
    rot.pitch = pool.random(
        std::uniform_real_distribution<coord_t>(0, numbers::TAU<coord_t>));
    rot.roll = pool.random(
        std::uniform_real_distribution<coord_t>(0, numbers::TAU<coord_t>));
    fireTime_ = pool.random(std::uniform_real_distribution<float>(0, 1));
}

bool EnemyPod::doEnemyTick(GameWorld& w, float delta) {
    fireTime_ += delta * 0.667f * w.difficulty().getFireRateMultiplier();
    killPlayerOnContact(w);
    rot += Orient3D(0.5, 0.05, 0.01) * delta;
    if (w.isPlayerAlive() &&
        pos.z - w.getPlayerPosition().z > getCollisionRadius()) {
        constexpr int N = 32;
        while (fireTime_ >= 1) {
            sendMessage(AudioMessage::playSound(SoundEffect::EnemyFire3,
                                                pos - w.getPlayerPosition()));
            const coord_t phi = numbers::PI<coord_t> * (3 - sqrt<coord_t>(5));
            coord_t y, r,
                theta = random(std::uniform_real_distribution<coord_t>(
                    0, numbers::TAU<coord_t>));
            for (int i = 0; i <= N; ++i) {
                y = lerp<coord_t>(-1, unlerp<coord_t>(0, i, N), 1);
                r = sqrt(1 - y * y);
                fireBullet<EnemyBullet>(
                    w, model().vertices[0],
                    Point3D(r * sin(theta), y, r * cos(theta)), 0.5f, 0.0f);
                theta += phi;
            }
            fireTime_ -= 1;
        }
    }
    return !isOffScreen();
}

bool EnemyPod::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    doExplode(w);
    sendMessage(AudioMessage::playSound(SoundEffect::ExplodeSmall,
                                        pos - w.getPlayerPosition()));
    if (killedByPlayer) {
        addScore(w, 500);
        w.onEnemyKilled(*this);
    }
    return true;
}

}  // namespace hiemalia
