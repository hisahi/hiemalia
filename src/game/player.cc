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
static const coord_t maxRotation = 0.1875;

#define R(x) ((x) * (x))
static const auto hitSpheres =
    hiemalia::to_array<CollisionSphere>({{{0, 0, 0}, R(0.046875)},
                                        {{0, 0, 0.03125}, R(0.015625)}});
#undef R

PlayerObject::PlayerObject() : GameObject() {
    model = getGameModel(GameModel::PlayerShip);
    setCollisionRadius(0.125);
}

bool PlayerObject::collideLineInternal(const ModelPoint& p1,
                                       const ModelPoint& p2) const {
    return std::any_of(hitSpheres.begin(), hitSpheres.end(),
                       [&](const CollisionSphere& sphere) {
                           return collidesLineSphere(
                               p1, p2, pos + sphere.center, sphere.radius2);
                       });
}

bool PlayerObject::collideCuboidInternal(const ModelPoint& c1,
                                         const ModelPoint& c2) const {
    return std::any_of(hitSpheres.begin(), hitSpheres.end(),
                       [&](const CollisionSphere& sphere) {
                           return collidesCuboidSphere(
                               c1, c2, pos + sphere.center, sphere.radius2);
                       });
}

bool PlayerObject::collideSphereInternal(const ModelPoint& p,
                                         coord_t r2) const {
    return std::any_of(hitSpheres.begin(), hitSpheres.end(),
                       [&](const CollisionSphere& sphere) {
                           return collidesSphereSphere(
                               p, r2, pos + sphere.center, sphere.radius2);
                       });
}

void PlayerObject::onDamage(float damage) {}

void PlayerObject::onDeath() {}

void PlayerObject::updateInput(ControlState& controls) { inputs = controls; }

void PlayerObject::inputsVelocity(float delta) {
    if (inputs.left) {
        vel.x = std::max<coord_t>(vel.x - maxSpeed * delta * 4, -maxSpeed);
    } else if (inputs.right) {
        vel.x = std::min<coord_t>(vel.x + maxSpeed * delta * 4, maxSpeed);
    } else if (vel.x < 0) {
        vel.x = std::min<coord_t>(0, vel.x + maxSpeed * delta * 4);
    } else if (vel.x > 0) {
        vel.x = std::max<coord_t>(0, vel.x - maxSpeed * delta * 4);
    }

    if (inputs.up) {
        vel.y = std::max<coord_t>(vel.y - maxSpeed * delta * 4, -maxSpeed);
    } else if (inputs.down) {
        vel.y = std::min<coord_t>(vel.y + maxSpeed * delta * 4, maxSpeed);
    } else if (vel.y < 0) {
        vel.y = std::min<coord_t>(0, vel.y + maxSpeed * delta * 4);
    } else if (vel.y > 0) {
        vel.y = std::max<coord_t>(0, vel.y - maxSpeed * delta * 4);
    }
}

void PlayerObject::inputsAngles(const MoveRegion& r, float delta) {
    if (inputs.left && pos.x > r.x0) {
        rot.roll =
            std::max<coord_t>(rot.roll - maxRotation * 4 * delta, -maxRotation);
    } else if (inputs.right && pos.x < r.x1) {
        rot.roll =
            std::min<coord_t>(rot.roll + maxRotation * 4 * delta, maxRotation);
    } else if (rot.roll < 0) {
        rot.roll = std::min<coord_t>(0, rot.roll + maxRotation * 4 * delta);
    } else if (rot.roll > 0) {
        rot.roll = std::max<coord_t>(0, rot.roll - maxRotation * 4 * delta);
    }

    if (inputs.up && pos.y > r.y0) {
        rot.pitch = std::max<coord_t>(rot.pitch - maxRotation * 4 * delta,
                                      -maxRotation);
    } else if (inputs.down && pos.y < r.y1) {
        rot.pitch =
            std::min<coord_t>(rot.pitch + maxRotation * 4 * delta, maxRotation);
    } else if (rot.pitch < 0) {
        rot.pitch = std::min<coord_t>(0, rot.pitch + maxRotation * 4 * delta);
    } else if (rot.pitch > 0) {
        rot.pitch = std::max<coord_t>(0, rot.pitch - maxRotation * 4 * delta);
    }
}

void PlayerObject::checkBounds(const MoveRegion& r, GameWorld& w) {
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
}

bool PlayerObject::update(GameWorld& w, float delta) {
    inputsVelocity(delta);
    pos += vel * delta;
    const MoveRegion& r = w.getPlayerMoveRegion();
    checkBounds(r, w);
    inputsAngles(r, delta);
    return true;
}

}  // namespace hiemalia
