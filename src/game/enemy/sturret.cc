/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/sturret.cc: implementation of EnemySpreadTurret

#include "game/enemy/sturret.hh"

#include "audio.hh"
#include "game/obstacle.hh"
#include "game/world.hh"
#include "hiemalia.hh"

namespace hiemalia {
EnemySpreadTurret::EnemySpreadTurret(const Point3D& p, const Orient3D& r)
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

const std::vector<ExtraCollision>& EnemySpreadTurret::exCollisions() const {
    return exCol_;
}

void EnemySpreadTurret::aim(GameWorld& w, float delta) {
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

void EnemySpreadTurret::render(SplinterBuffer& sbuf, Renderer3D& r3d) {
    EnemyObject::render(sbuf, r3d);
    r3d.renderModel(sbuf, pos, baseRot_, scale, *baseModel_.model);
}

void EnemySpreadTurret::onSpawn(GameWorld& w) {
    w.spawn<Obstacle>(pos, baseRot_, GameModel::EnemyTurretStand);
}

bool EnemySpreadTurret::doEnemyTick(GameWorld& w, float delta) {
    if (w.isPlayerAlive()) {
        aim(w, delta);
        if (rot != targetRot_) {
            rot = rot.tendTo(targetRot_, delta);
            // exCol_[0].rot.yaw = baseRot_.yaw = rot.yaw;
        }
        if (rot == targetRot_)
            fireTime_ += delta * 1.5f * w.difficulty().getFireRateMultiplier();
        if (pos.z - w.getPlayerPosition().z > getCollisionRadius()) {
            Point3D atPlayer = aimAtPlayer(w, 0.5f, 1.0f);
            while (fireTime_ >= 1) {
                sendMessage(AudioMessage::playSound(
                    SoundEffect::EnemyFire1, pos - w.getPlayerPosition()));
                fireBullet<EnemyBullet>(w, model().vertices[10], atPlayer, 0.5f,
                                        0.0f);
                fireBullet<EnemyBullet>(w, model().vertices[10],
                                        Orient3D(0.25, 0, 0).rotate(atPlayer),
                                        0.5f, 0.0f);
                fireBullet<EnemyBullet>(w, model().vertices[10],
                                        Orient3D(-0.25, 0, 0).rotate(atPlayer),
                                        0.5f, 0.0f);
                fireBullet<EnemyBullet>(w, model().vertices[10],
                                        Orient3D(0, 0.25, 0).rotate(atPlayer),
                                        0.5f, 0.0f);
                fireBullet<EnemyBullet>(w, model().vertices[10],
                                        Orient3D(0, -0.25, 0).rotate(atPlayer),
                                        0.5f, 0.0f);
                fireTime_ -= 1;
            }
        }
        killPlayerOnContact(w);
    }
    return !isOffScreen();
}

bool EnemySpreadTurret::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
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
