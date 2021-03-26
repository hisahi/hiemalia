/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/player.cc: implementation of PlayerObject

#include "game/player.hh"

#include "assets.hh"
#include "collide.hh"
#include "game/world.hh"

namespace hiemalia {
static const coord_t maxSpeed = 1.5;
static const coord_t maxZspeed = 0.75;
static const coord_t maxRotation = 0.25;

PlayerObject::PlayerObject() : GameObject() {
    model = getGameModel(GameModel::PlayerShip);
}

bool PlayerObject::collideLineInternal(const ModelPoint& p1,
                                       const ModelPoint& p2) const {
    return true;
}

bool PlayerObject::collideCuboidInternal(const ModelPoint& c1,
                                         const ModelPoint& c2) const {
    return true;
}

bool PlayerObject::collideSphereInternal(const ModelPoint& p,
                                         coord_t r2) const {
    return true;
}

void PlayerObject::onDamage(float damage) {}

void PlayerObject::onDeath() {}

void PlayerObject::updateInput(ControlState& controls) { inputs = controls; }

void PlayerObject::inputsVelocity(float delta) {
    if (inputs.up) {
        vel.y = std::max<coord_t>(vel.y - maxSpeed * delta * 4, -maxSpeed);
    } else if (inputs.down) {
        vel.y = std::min<coord_t>(vel.y + maxSpeed * delta * 4, maxSpeed);
    } else if (vel.y < 0) {
        vel.y = std::min<coord_t>(0, vel.y + maxSpeed * delta * 4);
    } else if (vel.y > 0) {
        vel.y = std::max<coord_t>(0, vel.y - maxSpeed * delta * 4);
    }

    if (inputs.left) {
        vel.x = std::max<coord_t>(vel.x - maxSpeed * delta * 4, -maxSpeed);
    } else if (inputs.right) {
        vel.x = std::min<coord_t>(vel.x + maxSpeed * delta * 4, maxSpeed);
    } else if (vel.x < 0) {
        vel.x = std::min<coord_t>(0, vel.x + maxSpeed * delta * 4);
    } else if (vel.x > 0) {
        vel.x = std::max<coord_t>(0, vel.x - maxSpeed * delta * 4);
    }

    if (inputs.back) {
        vel.z = std::max<coord_t>(vel.z - maxZspeed * delta * 4, -maxZspeed);
    } else if (inputs.forward) {
        vel.z = std::min<coord_t>(vel.z + maxZspeed * delta * 4, maxZspeed);
    } else if (vel.z < 0) {
        vel.z = std::min<coord_t>(0, vel.z + maxZspeed * delta * 4);
    } else if (vel.z > 0) {
        vel.z = std::max<coord_t>(0, vel.z - maxZspeed * delta * 4);
    }
}

void PlayerObject::inputsAngles(float delta) {
    if (inputs.up) {
        rot.pitch = std::max<coord_t>(rot.pitch - maxRotation * 4 * delta,
                                      -maxRotation);
    } else if (inputs.down) {
        rot.pitch =
            std::min<coord_t>(rot.pitch + maxRotation * 4 * delta, maxRotation);
    } else if (rot.pitch < 0) {
        rot.pitch = std::min<coord_t>(0, rot.pitch + maxRotation * 4 * delta);
    } else if (rot.pitch > 0) {
        rot.pitch = std::max<coord_t>(0, rot.pitch - maxRotation * 4 * delta);
    }

    if (inputs.left) {
        rot.roll =
            std::max<coord_t>(rot.roll - maxRotation * 4 * delta, -maxRotation);
    } else if (inputs.right) {
        rot.roll =
            std::min<coord_t>(rot.roll + maxRotation * 4 * delta, maxRotation);
    } else if (rot.roll < 0) {
        rot.roll = std::min<coord_t>(0, rot.roll + maxRotation * 4 * delta);
    } else if (rot.roll > 0) {
        rot.roll = std::max<coord_t>(0, rot.roll - maxRotation * 4 * delta);
    }
}

void PlayerObject::checkBounds(GameWorld& w) {
    const MoveRegion& r = w.getPlayerMoveRegion();
    coord_t zoff = w.progress_f;
    if (pos.x < r.x0) {
        if (vel.x < 0) vel.x = 0;
        pos.x = r.x0;
    } else if (pos.x > r.x1) {
        if (vel.x > 0) vel.x = 0;
        pos.x = r.x1;
    }
    if (pos.y < r.y0) {
        if (vel.y < 0) vel.y = 0;
        pos.y = r.y0;
    } else if (pos.y > r.y1) {
        if (vel.y > 0) vel.y = 0;
        pos.y = r.y1;
    }
    if (pos.z < 0 + zoff) {
        if (vel.z < 0) vel.z = 0;
        pos.z = 0 + zoff;
    } else if (pos.z > 0.5 + zoff) {
        if (vel.z > 0) vel.z = 0;
        pos.z = 0.5 + zoff;
    }
}

bool PlayerObject::update(GameWorld& w, float delta) {
    inputsVelocity(delta);
    inputsAngles(delta);
    pos += vel * delta;
    checkBounds(w);
    return true;
}

}  // namespace hiemalia
