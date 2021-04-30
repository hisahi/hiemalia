/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/wturret.cc: implementation of EnemyWheeledTurret

#include "game/enemy/wturret.hh"

#include "audio.hh"
#include "game/obstacle.hh"
#include "game/world.hh"
#include "hiemalia.hh"

namespace hiemalia {
static const coord_t width = 0.3;

EnemyWheeledTurret::EnemyWheeledTurret(const Point3D& p, const Orient3D& r)
    : EnemyObject(p, 5.0f),
      baseModel_(getGameModel(GameModel::EnemyWheeledTurret)),
      baseRot_(r + Orient3D::atPlayer),
      targetRot_(r + Orient3D::atPlayer),
      targetYaw_(targetRot_.yaw) {
    useGameModel(GameModel::EnemyTurretCannon);
    rot = r + Orient3D::atPlayer;
    pos += rot.rotate(model().vertices[9]);
    fireTime_ =
        getRandomPool().random(std::uniform_real_distribution<float>(0, 1));
    exCol_.emplace_back(baseModel_.collision, Point3D(0, 0, 0), baseRot_,
                        scale);
    height_ = baseModel_.model->vertices[20].y;
    canHitWalls(true);
}

const std::vector<ExtraCollision>& EnemyWheeledTurret::exCollisions() const {
    return exCol_;
}

void EnemyWheeledTurret::aim(GameWorld& w, float delta) {
    Orient3D old = targetRot_;
    Point3D offset = w.getPlayerPosition() - pos;
    Orient3D newPredicted = Orient3D::toPolar(
        old.direction(offset.length()) +
        Point3D(0, 0, delta * w.getMoveSpeed()) - (didMove_ ? delta : 0) * vel);
    Orient3D newActual = Orient3D::toPolar(offset);
    if (angleDifference(newActual.pitch, baseRot_.pitch) <
        -numbers::PI<coord_t>)
        newActual.pitch = wrapAngle(baseRot_.pitch - numbers::PI<coord_t>);
    else if (angleDifference(newActual.pitch, baseRot_.pitch) > 0)
        newActual.pitch = baseRot_.pitch;
    float penalty = static_cast<float>(newActual.offBy(newPredicted)) * 0.5f;
    fireTime_ = std::max<float>(0, fireTime_ - penalty);
    targetRot_ = newActual;
}

void EnemyWheeledTurret::moveAround(GameWorld& w, float delta) {
    vel = baseRot_.direction(0.5);
    auto region = w.getMoveRegionForZ(pos.z);
    if (pos.x + vel.x * delta - width <= region.x0) {
        targetYaw_ = random(
            std::uniform_real_distribution<coord_t>(0, numbers::PI<coord_t>));
    } else if (pos.x + vel.x * delta + width >= region.x1) {
        targetYaw_ = random(
            std::uniform_real_distribution<coord_t>(-numbers::PI<coord_t>, 0));
    }
    didMove_ = angleDifferenceAbs(baseRot_.yaw, targetYaw_) < 1e-5;
    coord_t adiff = angleDifference(targetYaw_, baseRot_.yaw);
    if (adiff != 0) {
        baseRot_ = baseRot_.tendTo(Orient3D(targetYaw_, 0, 0), delta);
    }

    if (didMove_) {
        pos += vel * delta;
    } else {
        vel *= 0;
    }
}

void EnemyWheeledTurret::render(SplinterBuffer& sbuf, Renderer3D& r3d) {
    EnemyObject::render(sbuf, r3d);
    r3d.renderModel(sbuf, pos, baseRot_, scale, *baseModel_.model);
}

void EnemyWheeledTurret::onSpawn(GameWorld& w) {
    pos.y = w.getMoveRegionForZ(pos.z).y1 - height_;
}

bool EnemyWheeledTurret::doEnemyTick(GameWorld& w, float delta) {
    if (w.isPlayerAlive()) {
        moveAround(w, delta);
        exCol_[0].rot.yaw = baseRot_.yaw;
        aim(w, delta);
        if (rot != targetRot_) {
            rot = rot.tendTo(targetRot_, delta);
        }
        if (didMove_ && rot == targetRot_)
            fireTime_ += delta * 1.5f * w.difficulty().getFireRateMultiplier();
        if (pos.z - w.getPlayerPosition().z > getCollisionRadius()) {
            while (fireTime_ >= 1) {
                sendMessage(AudioMessage::playSound(
                    SoundEffect::EnemyFire1, pos - w.getPlayerPosition()));
                fireBulletAtPlayer<EnemyBullet>(w, model().vertices[10], 0.5f,
                                                0.0625f, 1.0f);
                fireTime_ -= 1;
            }
        }
        killPlayerOnContact(w);
    }
    return !isOffScreen();
}

bool EnemyWheeledTurret::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    doExplode(w);
    sendMessage(AudioMessage::playSound(SoundEffect::ExplodeMedium,
                                        pos - w.getPlayerPosition()));
    w.explodeEnemy(*this, *baseModel_.model);
    if (killedByPlayer) {
        addScore(w, 500);
        w.onEnemyKilled(*this);
    }
    return true;
}

}  // namespace hiemalia
