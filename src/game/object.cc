/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/object.cc: implementation of GameObject

#include "game/object.hh"

#include <cmath>

#include "assets.hh"
#include "collide.hh"
#include "game/world.hh"

namespace hiemalia {

GameObject::GameObject(const Point3D& pos) : pos(pos), oldPos_(pos) {}

void GameObject::setPosition(const Point3D& p) {
    onMove(p);
    oldPos_ = pos;
    pos = p;
}

void GameObject::setPosition(coord_t x, coord_t y, coord_t z) {
    setPosition(Point3D(x, y, z));
}

void GameObject::move(const Point3D& p) { setPosition(pos + p); }

void GameObject::move(coord_t x, coord_t y, coord_t z) {
    move(Point3D(x, y, z));
}

void GameObject::render(SplinterBuffer& sbuf, Renderer3D& r3d) {
    if (model_ != nullptr) r3d.renderModel(sbuf, pos, rot, scale, *model_);
}

bool GameObject::isInRegion(GameWorld& w, coord_t extentLeft,
                            coord_t extentRight, coord_t extentTop,
                            coord_t extentBottom) const {
    const MoveRegion& r = w.getMoveRegionForZ(pos.z);
    return pos.x - extentLeft >= r.x0 && pos.x + extentRight <= r.x1 &&
           pos.y - extentTop >= r.y0 && pos.y + extentBottom <= r.y1;
}

Matrix3D GameObject::getObjectModelMatrix() const {
    return Renderer3D::getModelMatrix(pos, rot, scale);
}

coord_t GameObject::getCollisionRadius() const { return collideRadius_; }

void GameObject::setCollisionRadius(coord_t r) { collideRadius_ = r; }

bool GameObject::hits(const GameObject& obj) const {
    return collidesSphereSphere(pos, collideRadius_, obj.pos,
                                obj.collideRadius_) &&
           (hitsSweep(obj) || hitsInternal(obj));
}

bool GameObject::hitsSweep(const GameObject& obj) const {
    return obj.collision_ && collidesLineModel(oldPos_, pos, *obj.collision_,
                                               obj.getObjectModelMatrix());
}

bool GameObject::hitsInternal(const GameObject& obj) const {
    return collision_ && obj.collision_ &&
           collidesModelModel(*collision_, getObjectModelMatrix(),
                              *obj.collision_, obj.getObjectModelMatrix());
}

void GameObject::setModel(std::shared_ptr<const Model>&& model) {
    modelHolder_ = std::move(model);
    model_ = modelHolder_.get();
}

void GameObject::setCollision(
    std::shared_ptr<const ModelCollision>&& collision) {
    collisionHolder_ = std::move(collision);
    collision_ = collisionHolder_.get();
}

void GameObject::setModel(const Model& model) {
    model_ = &model;
    modelHolder_ = nullptr;
}

void GameObject::setCollision(const ModelCollision& collision) {
    collision_ = &collision;
    collisionHolder_ = nullptr;
}

void GameObject::noModel() {
    model_ = nullptr;
    modelHolder_ = nullptr;
}

void GameObject::noCollision() {
    collision_ = nullptr;
    collisionHolder_ = nullptr;
}

void GameObject::useGameModel(GameModel m, bool collision /* = true */) {
    const LoadedGameModel& gm = getGameModel(m);
    setModel(gm.model);
    if (collision) setCollision(gm.collision);
    setCollisionRadius(gm.radius);
}

bool GameObject::isOffScreen() const {
    return pos.z < 0 && pos.z > collideRadius_;
}

void GameObject::doMove(float delta, const Point3D& v) {
    oldPos_ = pos;
    pos += delta * v;
}

void GameObject::doMove(float delta) { doMove(delta, vel); }

bool GameObject::tick(GameWorld& w, float delta) {
    oldPos_ = pos;
    if (!update(w, delta)) return false;
    if (oldPos_ == pos) doMove(delta);
    return true;
}

float ObjectDamageable::getHealth() const { return health_; }

bool ObjectDamageable::damage(GameWorld& w, float damage,
                              const Point3D& pointOfContact) {
    bool dead = damage >= health_;
    health_ -= damage;
    onDamage(w, damage, pointOfContact);
    if (dead) onDeath(w);
    return dead;
}

bool collidesCuboidObject(const Point3D& c1, const Point3D& c2,
                          const GameObject& obj) {
    if (obj.hasCollision())
        return collidesCuboidModel(c1, c2, obj.collision(),
                                   obj.getObjectModelMatrix());
    else
        return collidesPointCuboid(obj.pos, c1, c2);
}

bool collidesSphereObject(const Point3D& c, coord_t r, const GameObject& obj) {
    if (obj.hasCollision())
        return collidesSphereModel(c, r, obj.collision(),
                                   obj.getObjectModelMatrix());
    else
        return collidesPointSphere(obj.pos, c, r);
}

bool collidesSweepSphereObject(const Point3D& c1, const Point3D& c2, coord_t r,
                               const GameObject& obj) {
    if (obj.hasCollision())
        return collidesSweepSphereModel(c1, c2, r, obj.collision(),
                                        obj.getObjectModelMatrix());
    else
        return collidesSweepSpherePoint(c1, c2, r, obj.pos);
}

}  // namespace hiemalia
