/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/wasp.cc: implementation of EnemyWasp

#include "game/enemy/wasp.hh"

#include "audio.hh"
#include "game/world.hh"
#include "hiemalia.hh"
#include "math.hh"
#include "random.hh"

namespace hiemalia {
static const Point3D wingOffset = Point3D(0, -0.046875, 0.0625);

EnemyWasp::EnemyWasp(const Point3D& pos) : EnemyObject(pos) {
    useGameModel(GameModel::EnemyWasp);
    rot = Orient3D::atPlayer;
    fireTime_ = random(std::uniform_real_distribution<float>(0, 1));
    vel = Point3D(0, 0, -1 / 16);
    rot = Orient3D::atPlayer;
    wingModel_ = getGameModel(GameModel::EnemyWaspWing);
    exCol_.emplace_back(wingModel_.collision, pos, rot, Point3D(1, 1, 1));
    exCol_.emplace_back(wingModel_.collision, pos, rot, Point3D(-1, 1, 1));
}

bool EnemyWasp::doEnemyTick(GameWorld& w, float delta) {
    anim_ = frac(anim_ + delta * 1.25);
    if (anim_ >= 0.5) {
        pos.y -= 0.25 * delta;
    } else {
        pos.y += 0.25 * delta;
    }
    rot.roll = sin(pos.z * 0.1) / 8;
    fireTime_ += delta * 0.8f * w.difficulty().getFireRateMultiplier();
    killPlayerOnContact(w);
    if (w.isPlayerAlive() &&
        pos.z - w.getPlayerPosition().z > getCollisionRadius()) {
        while (fireTime_ >= 1) {
            sendMessage(AudioMessage::playSound(SoundEffect::EnemyFire3,
                                                pos - w.getPlayerPosition()));
            fireBulletAtPlayer<EnemyBulletHoming>(w, model().vertices[0],
                                                  0.625f, 0.25f, 0.0f);
            fireTime_ -= 1;
        }
    }
    coord_t p;
    if (anim_ >= 0.5)
        p = lerp(0.5, anim_ * 2 - 1, -0.5);
    else
        p = lerp(-0.5, anim_ * 2, 0.5);
    exCol_[0].pos = pos + wingOffset;
    exCol_[0].rot = rot + Orient3D(0, 0, -p);
    exCol_[1].pos = pos + wingOffset;
    exCol_[1].rot = rot + Orient3D(0, 0, p);
    return !isOffScreen();
}

void EnemyWasp::render(SplinterBuffer& sbuf, Renderer3D& r3d) {
    EnemyObject::render(sbuf, r3d);
    renderExCol(sbuf, r3d, exCol_[0], *wingModel_.model);
    renderExCol(sbuf, r3d, exCol_[1], *wingModel_.model);
}

const std::vector<ExtraCollision>& EnemyWasp::exCollisions() const {
    return exCol_;
}

bool EnemyWasp::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    doExplode(w);
    sendMessage(AudioMessage::playSound(SoundEffect::ExplodeSmall,
                                        pos - w.getPlayerPosition()));
    if (killedByPlayer) {
        addScore(w, 400);
        w.onEnemyKilled(*this);
    }
    return true;
}

}  // namespace hiemalia
