/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/turret.cc: implementation of EnemyTurret

#include "game/enemy/turret.hh"

#include "audio.hh"
#include "game/obstacle.hh"
#include "game/world.hh"
#include "hiemalia.hh"

namespace hiemalia {
EnemyTurret::EnemyTurret(const Point3D& p, const Orient3D& r)
    : EnemyObject(p, 4.0f),
      baseModel_(getGameModel(GameModel::EnemyTurret)),
      baseRot_(r + Orient3D::atPlayer),
      targetRot_(r + Orient3D::atPlayer) {
    useGameModel(GameModel::EnemyTurretCannon);
    rot = r + Orient3D::atPlayer;
    pos += rot.rotate(model().vertices[9]);
    fireTime_ =
        getRandomPool().random(std::uniform_real_distribution<float>(0, 1));
    exCol_.emplace_back(baseModel_.collision, Point3D(0, 0, 0), baseRot_,
                        scale);
}

const std::vector<ExtraCollision>& EnemyTurret::exCollisions() const {
    return exCol_;
}

void EnemyTurret::aim(GameWorld& w, float delta) {
    Orient3D old = targetRot_;
    Point3D offset = w.getPlayerPosition() - pos;
    Orient3D newPredicted =
        Orient3D::toPolar(old.direction(offset.length()) +
                          Point3D(0, 0, delta * w.getMoveSpeed()));
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

void EnemyTurret::render(SplinterBuffer& sbuf, Renderer3D& r3d) {
    EnemyObject::render(sbuf, r3d);
    r3d.renderModel(sbuf, pos, baseRot_, scale, *baseModel_.model);
}

void EnemyTurret::onSpawn(GameWorld& w) {
    w.spawn<Obstacle>(pos, baseRot_, GameModel::EnemyTurretStand);
}

bool EnemyTurret::doEnemyTick(GameWorld& w, float delta) {
    if (w.isPlayerAlive()) {
        aim(w, delta);
        if (rot != targetRot_) {
            rot = rot.tendTo(targetRot_, delta);
            // exCol_[0].rot.yaw = baseRot_.yaw = rot.yaw;
        }
        if (rot == targetRot_)
            fireTime_ += delta * 1.5f * w.difficulty().getFireRateMultiplier();
        if (pos.z - w.getPlayerPosition().z > getCollisionRadius()) {
            while (fireTime_ >= 1) {
                sendMessage(AudioMessage::playSound(
                    SoundEffect::EnemyFire1, pos - w.getPlayerPosition()));
                fireBulletAtPlayer<EnemyBulletSimple>(w, model().vertices[10],
                                                      0.5f, 0.0625f, 1.0f);
                fireTime_ -= 1;
            }
        }
        killPlayerOnContact(w);
    }
    return !isOffScreen();
}

bool EnemyTurret::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    doExplode(w);
    sendMessage(AudioMessage::playSound(SoundEffect::ExplodeMedium,
                                        pos - w.getPlayerPosition()));
    w.explodeEnemy(*this, *baseModel_.model);
    if (killedByPlayer) {
        addScore(w, 400);
        w.onEnemyKilled(*this);
    }
    return true;
}

}  // namespace hiemalia
