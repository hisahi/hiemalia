/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/sbox.cc: implementation of SlidingBox

#include "game/sbox.hh"

#include <algorithm>
#include <cmath>

#include "assets.hh"
#include "collide.hh"
#include "game/enemy.hh"
#include "game/world.hh"
#include "math.hh"

namespace hiemalia {
SlidingBox::SlidingBox(const Point3D& pos, coord_t dir, coord_t x0, coord_t y0,
                       coord_t z0, coord_t x1, coord_t y1, coord_t z1,
                       coord_t t, coord_t p)
    : GameObject(pos),
      x_(t),
      v_(p),
      p0_(x0, y0, z0),
      p1_(x1, y1, z1),
      pos_(0, 0, 0) {
    model_ = getGameModel(GameModel::SlidingBoxModel).model;
    collision_ = std::make_shared<ModelCollision>();
    setModel(model_);
    setCollision(collision_);
    setCollisionRadius((p1_ - p0_).length() / 2);
    scale = Point3D(1, 1, std::abs(z0 - z1) / 2);

    int d = remainder(static_cast<int>(dir), 2);
    bool inv = false;
    switch (d) {
        case 0:
            inv = x0 > x1;
            isY_ = false;
            break;
        case 1:
            inv = y0 > y1;
            isY_ = true;
            break;
    }
    if (inv) {
        if (isY_ && p0_.x < p1_.x) std::swap(p0_.x, p1_.x);
        if (!isY_ && p0_.y < p1_.y) std::swap(p0_.y, p1_.y);
        pmin_ = &pos_, pmax_ = &p0_;
    } else {
        if (isY_ && p0_.x > p1_.x) std::swap(p0_.x, p1_.x);
        if (!isY_ && p0_.y > p1_.y) std::swap(p0_.y, p1_.y);
        pmin_ = &p0_, pmax_ = &pos_;
    }
    pos_ = p1_;
    lerpA_ = isY_ ? p0_.y : p0_.x;
    lerpB_ = isY_ ? p1_.y : p1_.x;
    collision_->shapes.push_back(CollisionShape::cuboid(*pmin_, *pmax_));
    collision_->shapes[0].p.z = std::min(z0, z1);
    collision_->shapes[0].p1.z = std::max(z0, z1);
}

void SlidingBox::absorbBullets(GameWorld& w, const BulletList& list) {
    for (auto& bptr : list) {
        if (hits(*bptr)) {
            bptr->backtrackCuboid(pos + *pmin_, pos + *pmax_);
            bptr->impact(w, false);
        }
    }
}

void SlidingBox::absorbEnemies(GameWorld& w, const EnemyList& list,
                               const Point3D& avg, const Point3D& siz) {
    for (auto& e : list) {
        if (e->hits(*this)) {
            Point3D dir = collidesCuboidPointDirection(e->pos, avg, siz);
            e->hitWall(w, dir.x, dir.y, dir.z);
        }
    }
}

void SlidingBox::updateBox() {
    auto& v = model_->vertices;
    coord_t x0 = pmin_->x, x1 = pmax_->x;
    coord_t y0 = pmin_->y, y1 = pmax_->y;
    v[0].x = x1, v[0].y = y1;
    v[1].x = x1, v[1].y = y1;
    v[2].x = x0, v[2].y = y1;
    v[3].x = x0, v[3].y = y1;
    v[4].x = x1, v[4].y = y0;
    v[5].x = x1, v[5].y = y0;
    v[6].x = x0, v[6].y = y0;
    v[7].x = x0, v[7].y = y0;
    v[8].x = x1, v[8].y = y1;
    v[9].x = x0, v[9].y = y0;
    v[10].x = x0, v[10].y = y1;
    v[11].x = x1, v[11].y = y0;
    v[12].x = x1, v[12].y = y1;
    v[13].x = x0, v[13].y = y0;
    v[14].x = x0, v[14].y = y1;
    v[15].x = x1, v[15].y = y0;

    collision_->shapes[0].p.x = x0;
    collision_->shapes[0].p.y = y0;
    collision_->shapes[0].p1.x = x1;
    collision_->shapes[0].p1.y = y1;
}

coord_t SlidingBox::getLerp(coord_t t) { return t >= 0.5 ? 2 - t * 2 : t * 2; }

bool SlidingBox::update(GameWorld& w, float delta) {
    (isY_ ? pos_.y : pos_.x) = lerp(lerpA_, getLerp(x_), lerpB_);
    updateBox();
    x_ = frac(x_ + v_ * w.difficulty().getBulletSpeedMultiplier() * delta);
    Point3D avg = Point3D::average(pos + *pmin_, pos + *pmax_);
    Point3D siz = Point3D::average(-*pmin_, *pmax_);
    if (w.isPlayerAlive() && w.getPlayer().hits(*this)) {
        Point3D dir =
            collidesCuboidPointDirection(w.getPlayerPosition(), avg, siz);
        w.getPlayer().wallContact(dir.x, dir.y, dir.z);
    }
    absorbEnemies(w, w.getEnemies(), avg, siz);
    absorbBullets(w, w.getPlayerBullets());
    absorbBullets(w, w.getEnemyBullets());
    return !isOffScreen();
}

SlidingBoxSine::SlidingBoxSine(const Point3D& pos, coord_t dir, coord_t x0,
                               coord_t y0, coord_t z0, coord_t x1, coord_t y1,
                               coord_t z1, coord_t t, coord_t p)
    : SlidingBox(pos, dir, x0, y0, z0, x1, y1, z1, t, p) {}

coord_t SlidingBoxSine::getLerp(coord_t t) {
    return (sin(t * numbers::TAU<coord_t>) + 1) * 0.5;
}
}  // namespace hiemalia
