/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/player.cc: implementation of PlayerObject

#include "game/player.hh"

#include "assets.hh"
#include "audio.hh"
#include "collide.hh"
#include "game/pbullet.hh"
#include "game/world.hh"
#include "hiemalia.hh"
#include "random.hh"

namespace hiemalia {
static const coord_t maxSpeed = 1.5;
static const coord_t maxRotation = 0.1875;

static std::uniform_real_distribution<coord_t> rd_rotmul(0.5, 1.0);
static std::uniform_real_distribution<coord_t> rd_yawmul(-0.5, 1.0);

PlayerObject::PlayerObject() : GameObject() {
    useGameModel(GameModel::PlayerShip);
    shipRadius_ = getCollisionRadius();
}

void PlayerObject::onDamage(GameWorld& w, float damage,
                            const ModelPoint& contact) {
    if (woundedBird_) return;
    wbird_vel_.yaw = (contact.x - pos.x) * -8 * random(rd_yawmul);
    wbird_vel_.pitch = (contact.z - pos.z) * 12 * random(rd_rotmul);
    wbird_vel_.roll = (contact.x - pos.x) * 32 * random(rd_rotmul);
    wbird_fr_ = 0;
    wbird_mul_ = std::max(0.25, 0.25 + (contact.z - pos.z) * 12) * 0.125;
}

void PlayerObject::onDeath(GameWorld& w) {
    if (woundedBird_) return;
    sendMessage(AudioMessage::playSound(SoundEffect::PlayerHit));
    woundedBird_ = true;
}

void PlayerObject::enemyContact() { wallContact(0, 0, 0); }

void PlayerObject::wallContact(coord_t x, coord_t y, coord_t z) {
    explodeObject_ = std::make_unique<Explosion>(*this, x, y, z, 1.0f);
}

void PlayerObject::updateInput(ControlState& controls) { inputs_ = controls; }

void PlayerObject::inputsVelocity(float delta) {
    if (inputs_.left) {
        vel.x = std::max<coord_t>(vel.x - maxSpeed * delta * 4, -maxSpeed);
    } else if (inputs_.right) {
        vel.x = std::min<coord_t>(vel.x + maxSpeed * delta * 4, maxSpeed);
    } else if (vel.x < 0) {
        vel.x = std::min<coord_t>(0, vel.x + maxSpeed * delta * 4);
    } else if (vel.x > 0) {
        vel.x = std::max<coord_t>(0, vel.x - maxSpeed * delta * 4);
    }

    if (inputs_.up) {
        vel.y = std::max<coord_t>(vel.y - maxSpeed * delta * 4, -maxSpeed);
    } else if (inputs_.down) {
        vel.y = std::min<coord_t>(vel.y + maxSpeed * delta * 4, maxSpeed);
    } else if (vel.y < 0) {
        vel.y = std::min<coord_t>(0, vel.y + maxSpeed * delta * 4);
    } else if (!woundedBird_ && vel.y > 0) {
        vel.y = std::max<coord_t>(0, vel.y - maxSpeed * delta * 4);
    }
}

void PlayerObject::inputsAngles(float delta) {
    if (inputs_.left) {
        rot.roll =
            std::max<coord_t>(rot.roll - maxRotation * 4 * delta, -maxRotation);
    } else if (inputs_.right) {
        rot.roll =
            std::min<coord_t>(rot.roll + maxRotation * 4 * delta, maxRotation);
    } else if (rot.roll < 0) {
        rot.roll = std::min<coord_t>(0, rot.roll + maxRotation * 4 * delta);
    } else if (rot.roll > 0) {
        rot.roll = std::max<coord_t>(0, rot.roll - maxRotation * 4 * delta);
    }

    if (inputs_.up) {
        rot.pitch = std::max<coord_t>(rot.pitch - maxRotation * 4 * delta,
                                      -maxRotation);
    } else if (inputs_.down) {
        rot.pitch =
            std::min<coord_t>(rot.pitch + maxRotation * 4 * delta, maxRotation);
    } else if (rot.pitch < 0) {
        rot.pitch = std::min<coord_t>(0, rot.pitch + maxRotation * 4 * delta);
    } else if (rot.pitch > 0) {
        rot.pitch = std::max<coord_t>(0, rot.pitch - maxRotation * 4 * delta);
    }
}

void PlayerObject::checkBounds(const MoveRegion& r, GameWorld& w) {
    if (pos.y - shipRadius_ < r.y0) {
        if (vel.y < 0) vel.y = 0;
        pos.y = r.y0;
        wallContact(0, 1, 0);
        return;
    } else if (pos.y + shipRadius_ > r.y1) {
        if (vel.y > 0) vel.y = 0;
        pos.y = r.y1;
        wallContact(0, -1, 0);
        return;
    } else if (pos.x - shipRadius_ < r.x0) {
        if (vel.x < 0) vel.x = 0;
        pos.x = r.x0;
        wallContact(1, 0, 0);
        return;
    } else if (pos.x + shipRadius_ > r.x1) {
        if (vel.x > 0) vel.x = 0;
        pos.x = r.x1;
        wallContact(-1, 0, 0);
        return;
    }
}

bool PlayerObject::playerInControl() const { return !woundedBird_; }

void PlayerObject::doWoundedBird(float delta) {
    wbird_fr_ += delta;
    vel.y += wbird_fr_ * wbird_fr_ * wbird_mul_;
    rot += wbird_vel_ * delta;
}

void PlayerObject::doFire(GameWorld& w) {
    fireInterval_ = 0.2f;
    sendMessage(AudioMessage::playSound(SoundEffect::PlayerFire));
    Matrix3D mat = getObjectModelMatrix();
    ModelPoint leftTurret = mat.project(ModelPoint(-2, 0.0625, 0.25) * 0.0625);
    ModelPoint rightTurret = mat.project(ModelPoint(2, 0.0625, 0.25) * 0.0625);
    w.firePlayerBullet<PlayerBullet>(leftTurret, ModelPoint(0.125, 0, 4) * 1.5);
    w.firePlayerBullet<PlayerBullet>(rightTurret,
                                     ModelPoint(-0.125, 0, 4) * 1.5);
}

bool PlayerObject::update(GameWorld& w, float delta) {
    if (explodeObject_) {
        auto obj = w.explodePlayer(std::move(explodeObject_));
        return true;
    }
    inputsVelocity(delta);
    pos += vel * delta;
    if (fireInterval_ >= 0) fireInterval_ -= delta;
    const MoveRegion& r = w.getPlayerMoveRegion();
    checkBounds(r, w);
    if (woundedBird_)
        doWoundedBird(delta);
    else {
        inputsAngles(delta);
        if (fireInterval_ <= 0 && inputs_.fire) doFire(w);
    }
    return true;
}

}  // namespace hiemalia
