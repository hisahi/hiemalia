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
BulletObject::BulletObject(const Point3D& pos) : GameObject(pos) {}

bool BulletObject::update(GameWorld& w, float delta) {
    bool alive = doBulletTick(w, delta);
    if (!alive) return false;
    Orient3D frotvel = rotvel * delta;
    if (!checkInBounds(w)) return false;
    rot += frotvel;
    return alive_ && !isOffScreen2();
}

void BulletObject::onMove(const Point3D& newPos) { oldPos_ = pos; }

void BulletObject::kill(GameWorld& w) { alive_ = false; }

bool BulletObject::hitsSweep(const GameObject& obj) const {
    return false;  // use hitsInternal
}

bool BulletObject::hitsInternal(const GameObject& obj) const {
    return collidesSweepSphereObject(oldPos_, pos, getCollisionRadius(), obj);
}

Point3D BulletObject::lerp(coord_t p) const {
    return Point3D::lerp(oldPos_, p, pos);
}

void BulletObject::backtrackCuboid(const Point3D& c1, const Point3D& c2) {
    pos = collidesSweepSphereCuboidWhere(oldPos_, pos, getCollisionRadius(), c1,
                                         c2);
}

void BulletObject::backtrackSphere(const Point3D& p, coord_t r2) {
    pos = collidesSweepSphereSphereWhere(oldPos_, pos, getCollisionRadius(), p,
                                         r2);
}

void BulletObject::backtrackModel(const ModelCollision& mc,
                                  const Matrix3D mat) {
    pos = collidesSweepSphereModelWhere(oldPos_, pos, getCollisionRadius(), mc,
                                        mat);
}

void BulletObject::backtrackObject(const GameObject& o) {
    if (o.hasCollision())
        backtrackModel(o.collision(), o.getObjectModelMatrix());
}

bool BulletObject::checkInBounds(GameWorld& w) {
    const MoveRegion& r = w.getMoveRegionForZ(pos.z);
    if ((pos.y < r.y0) || (pos.y > r.y1) || (pos.x < r.x0) || (pos.x > r.x1)) {
        impact(w, false);
        dynamic_assert(!alive_, "an impact should kill the bullet");
        return false;
    }
    return true;
}
}  // namespace hiemalia
