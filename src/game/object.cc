/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/object.cc: implementation of GameObject

#include "game/object.hh"

#include <cmath>

#include "collide.hh"

namespace hiemalia {

void GameObject::setPosition(const ModelPoint& p) {
    pos = p;
    onSetPosition();
}

void GameObject::setPosition(coord_t x, coord_t y, coord_t z) {
    pos = ModelPoint(x, y, z);
    onSetPosition();
}

void GameObject::move(const ModelPoint& p) { pos += p; }

void GameObject::move(coord_t x, coord_t y, coord_t z) {
    pos += ModelPoint(x, y, z);
}

void GameObject::render(SplinterBuffer& sbuf, Renderer3D& r3d) {
    if (model == nullptr)
        renderSpecial(sbuf, r3d);
    else
        r3d.renderModel(sbuf, pos, rot, scale, *model);
}

Matrix3D GameObject::getObjectModelMatrix() const {
    return Renderer3D::getModelMatrix(pos, rot, scale);
}

coord_t GameObject::getCollisionRadiusSquared() const {
    return collideRadiusSquared_;
}

coord_t GameObject::getCollisionRadius() const {
    return std::sqrt(getCollisionRadiusSquared());
}

void GameObject::setCollisionRadius(coord_t r) {
    collideRadiusSquared_ = r * r;
}

bool GameObject::collideLine(const ModelPoint& p1, const ModelPoint& p2) const {
    return collidesLineSphere(p1, p2, pos, collideRadiusSquared_) &&
           collideLineInternal(p1, p2);
}

bool GameObject::collideCuboid(const ModelPoint& c1,
                               const ModelPoint& c2) const {
    return collidesSphereCuboid(pos, collideRadiusSquared_, c1, c2) &&
           collideCuboidInternal(c1, c2);
}

bool GameObject::collideSphere(const ModelPoint& p, coord_t r2) const {
    return collidesSphereSphere(pos, collideRadiusSquared_, p, r2) &&
           collideSphereInternal(p, r2);
}

bool GameObject::isOffScreen() const {
    return pos.z < 0 && pos.z * pos.z > collideRadiusSquared_;
}

float ObjectDamageable::getHealth() const { return health_; }

bool ObjectDamageable::damage(GameWorld& w, float damage,
                              const ModelPoint& pointOfContact) {
    bool dead = damage >= health_;
    health_ -= damage;
    onDamage(w, damage, pointOfContact);
    if (dead) onDeath(w);
    return dead;
}

}  // namespace hiemalia
