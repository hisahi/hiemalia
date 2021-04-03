/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/bullet.cc: implementation of base bullet object

#include "game/bullet.hh"

#include <cmath>
#include <random>

#include "assets.hh"
#include "collide.hh"
#include "game/world.hh"
#include "math.hh"
#include "model.hh"
#include "rend3d.hh"

namespace hiemalia {
BulletObject::BulletObject() {}

bool BulletObject::update(GameWorld& w, float delta) {
    oldPos_ = pos;
    bool alive = doBulletTick(w, delta);
    if (!alive) return false;
    ModelPoint fvel = vel * delta;
    Rotation3D frotvel = rotvel * delta;
    if (!checkInBounds(w, fvel)) return false;
    pos += fvel;
    rot += frotvel;
    return alive_ && !isBulletOffScreen();
}

void BulletObject::onSetPosition() { oldPos_ = pos; }

void BulletObject::kill(GameWorld& w) { alive_ = false; }

bool BulletObject::hitsInternal(const GameObject& obj) const {
    return collidesSphereObject(pos, getCollisionRadius(), obj);
}

ModelPoint BulletObject::lerp(coord_t p) const {
    return lerpPoint(oldPos_, p, pos);
}

void BulletObject::backtrackCuboid(const ModelPoint& c1, const ModelPoint& c2) {
    pos = collidesLineCuboidWhere(oldPos_, pos, c1, c2);
}

void BulletObject::backtrackSphere(const ModelPoint& p, coord_t r2) {
    pos = collidesLineSphereWhere(oldPos_, pos, p, r2);
}

bool BulletObject::checkInBounds(GameWorld& w, const ModelPoint& fvel) {
    const MoveRegion& r = w.getMoveRegionForZ(pos.z);
    if ((pos.y + fvel.y < r.y0) || (pos.y + fvel.y > r.y1) ||
        (pos.x + fvel.x < r.x0) || (pos.x + fvel.x > r.x1)) {
        impact(w, false);
        dynamic_assert(!alive_, "an impact should kill the bullet");
        return false;
    }
    return true;
}

bool BulletObject::isBulletOffScreen() const {
    if (vel.z < 0)
        return isOffScreen();
    else
        return pos.z > stageSpawnDistance + getCollisionRadius();
}
}  // namespace hiemalia
