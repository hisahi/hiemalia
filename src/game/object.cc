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
    return obj.collision_ && collidesLineObject(oldPos_, pos, obj);
}

bool GameObject::hitsInternal(const GameObject& obj) const {
    return collision_ && obj.collision_ && collidesObjectObject(*this, obj);
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

bool GameObject::isOffScreen() const { return pos.z < -collideRadius_; }

bool GameObject::isOffScreen2() const {
    return isOffScreen() ||
           (vel.z > 0 && pos.z >= farObjectBackPlane + collideRadius_);
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

const std::vector<ExtraCollision>& GameObject::exCollisions() const {
    static const std::vector<ExtraCollision> exCollisionsNone{};
    return exCollisionsNone;
}

void GameObject::renderExCol(SplinterBuffer& sbuf, Renderer3D& r3d,
                             const ExtraCollision& col,
                             const Model& model) const {
    r3d.renderModel(sbuf, col.pos, col.rot, col.scale, model);
}

float ObjectDamageable::getHealth() const { return health_; }

void ObjectDamageable::setHealth(float h) { health_ = h; }

bool ObjectDamageable::damage(GameWorld& w, float damage,
                              const Point3D& pointOfContact) {
    bool dead = damage >= health_;
    health_ -= damage;
    onDamage(w, damage, pointOfContact);
    if (dead) onDeath(w);
    return dead;
}

static bool collidesLineObjectBasic(const Point3D& l1, const Point3D& l2,
                                    const GameObject& obj) {
    return obj.hasCollision() && collidesLineModel(l1, l2, obj.collision(),
                                                   obj.getObjectModelMatrix());
}

static bool collidesCuboidObjectBasic(const Point3D& c1, const Point3D& c2,
                                      const GameObject& obj) {
    if (obj.hasCollision())
        return collidesCuboidModel(c1, c2, obj.collision(),
                                   obj.getObjectModelMatrix());
    else
        return collidesPointCuboid(obj.pos, c1, c2);
}

static bool collidesSphereObjectBasic(const Point3D& c, coord_t r,
                                      const GameObject& obj) {
    if (obj.hasCollision())
        return collidesSphereModel(c, r, obj.collision(),
                                   obj.getObjectModelMatrix());
    else
        return collidesPointSphere(obj.pos, c, r);
}

static bool collidesSweepSphereObjectBasic(const Point3D& c1, const Point3D& c2,
                                           coord_t r, const GameObject& obj) {
    if (obj.hasCollision())
        return collidesSweepSphereModel(c1, c2, r, obj.collision(),
                                        obj.getObjectModelMatrix());
    else
        return collidesSweepSpherePoint(c1, c2, r, obj.pos);
}

bool collidesLineObject(const Point3D& l1, const Point3D& l2,
                        const GameObject& obj) {
    if (collidesLineObjectBasic(l1, l2, obj)) return true;
    if (!obj.exCollisions().empty()) {
        Point3D bpos = obj.pos;
        const auto& ex = obj.exCollisions();
        return std::any_of(
            ex.begin(), ex.end(), [&l1, &l2, &bpos](const ExtraCollision& c) {
                return collidesLineModel(
                    l1, l2, *c.collision,
                    Renderer3D::getModelMatrix(bpos + c.pos, c.rot, c.scale));
            });
    }
    return false;
}

bool collidesCuboidObject(const Point3D& c1, const Point3D& c2,
                          const GameObject& obj) {
    if (collidesCuboidObjectBasic(c1, c2, obj)) return true;
    if (!obj.exCollisions().empty()) {
        Point3D bpos = obj.pos;
        const auto& ex = obj.exCollisions();
        return std::any_of(
            ex.begin(), ex.end(), [&c1, &c2, &bpos](const ExtraCollision& c) {
                return collidesCuboidModel(
                    c1, c2, *c.collision,
                    Renderer3D::getModelMatrix(bpos + c.pos, c.rot, c.scale));
            });
    }
    return false;
}

bool collidesSphereObject(const Point3D& c, coord_t r, const GameObject& obj) {
    if (collidesSphereObjectBasic(c, r, obj)) return true;
    if (!obj.exCollisions().empty()) {
        Point3D bpos = obj.pos;
        const auto& ex = obj.exCollisions();
        return std::any_of(ex.begin(), ex.end(),
                           [&c, &r, &bpos](const ExtraCollision& cx) {
                               return collidesSphereModel(
                                   c, r, *cx.collision,
                                   Renderer3D::getModelMatrix(
                                       bpos + cx.pos, cx.rot, cx.scale));
                           });
    }
    return false;
}

bool collidesSweepSphereObject(const Point3D& c1, const Point3D& c2, coord_t r,
                               const GameObject& obj) {
    if (c1 == c2) return collidesSphereObject(c1, r, obj);
    if (collidesSweepSphereObjectBasic(c1, c2, r, obj)) return true;
    if (!obj.exCollisions().empty()) {
        Point3D bpos = obj.pos;
        const auto& ex = obj.exCollisions();
        return std::any_of(
            ex.begin(), ex.end(),
            [&c1, &c2, &r, &bpos](const ExtraCollision& c) {
                return collidesSweepSphereModel(
                    c1, c2, r, *c.collision,
                    Renderer3D::getModelMatrix(bpos + c.pos, c.rot, c.scale));
            });
    }
    return false;
}

static bool collidesObjectObjectBasic(const GameObject& obj1,
                                      const GameObject& obj2) {
    if (obj1.hasCollision() && obj2.hasCollision())
        return collidesModelModel(obj1.collision(), obj1.getObjectModelMatrix(),
                                  obj2.collision(),
                                  obj2.getObjectModelMatrix());
    else
        return false;
}

bool collidesExModelObject(const ModelCollision& c1, const Matrix3D& mat1,
                           const GameObject& obj2) {
    if (obj2.hasCollision() && collidesModelModel(c1, mat1, obj2.collision(),
                                                  obj2.getObjectModelMatrix()))
        return true;
    const auto& ex2 = obj2.exCollisions();
    Point3D bpos2 = obj2.pos;
    return std::any_of(
        ex2.begin(), ex2.end(), [&bpos2, &c1, &mat1](const ExtraCollision& c) {
            return collidesModelModel(
                c1, mat1, *c.collision,
                Renderer3D::getModelMatrix(bpos2 + c.pos, c.rot, c.scale));
        });
}

bool collidesObjectObject(const GameObject& obj1, const GameObject& obj2) {
    if (!obj1.exCollisions().empty() || !obj2.exCollisions().empty()) {
        const auto& ex1 = obj1.exCollisions();
        Point3D bpos1 = obj1.pos;
        return collidesExModelObject(obj1.collision(),
                                     obj1.getObjectModelMatrix(), obj2) ||
               std::any_of(ex1.begin(), ex1.end(),
                           [&bpos1, &obj2](const ExtraCollision& c) {
                               return collidesExModelObject(
                                   *c.collision,
                                   Renderer3D::getModelMatrix(bpos1 + c.pos,
                                                              c.rot, c.scale),
                                   obj2);
                           });
    }
    return collidesObjectObjectBasic(obj1, obj2);
}

}  // namespace hiemalia
