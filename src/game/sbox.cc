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
        pmin_ = &pos_;
        pmax_ = &p1_;
        pos_ = p0_;
    } else {
        pmin_ = &p0_;
        pmax_ = &pos_;
        pos_ = p1_;
    }
    collision_->shapes.push_back(CollisionShape::cuboid(*pmin_, *pmax_));
}

void SlidingBox::absorbBullets(GameWorld& w, const BulletList& list) {
    for (auto& bptr : list) {
        if (hits(*bptr)) {
            bptr->backtrackCuboid(pos + *pmin_, pos + *pmax_);
            bptr->impact(w, false);
        }
    }
}

void SlidingBox::absorbEnemies(GameWorld& w, const EnemyList& list) {
    for (auto& e : list) {
        if (e->hits(*this)) {
            e->hitWall(w);
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
    if (isY_)
        pos_.y = lerp(p0_.y, getLerp(x_), p1_.y);
    else
        pos_.x = lerp(p0_.x, getLerp(x_), p1_.x);
    updateBox();
    x_ = frac(x_ + v_ * delta);
    if (w.isPlayerAlive() && w.getPlayer().hits(*this)) {
        const Point3D& ppos = w.getPlayer().pos;
        coord_t dx = (ppos.x - pos.x) / scale.x;
        coord_t dy = (ppos.y - pos.y) / scale.y;
        coord_t dz = (ppos.z - pos.z) / scale.z;
        if (std::abs(dz) >= std::abs(dx) && std::abs(dz) >= std::abs(dy))
            dx = dy = 0;
        else if (std::abs(dx) >= std::abs(dy) && std::abs(dx) >= std::abs(dz))
            dy = dz = 0;
        else if (std::abs(dy) >= std::abs(dx) && std::abs(dy) >= std::abs(dz))
            dx = dz = 0;
        w.getPlayer().wallContact(dx, dy, dz);
    }
    absorbEnemies(w, w.getEnemies());
    absorbBullets(w, w.getPlayerBullets());
    absorbBullets(w, w.getEnemyBullets());
    return !isOffScreen();
}

SlidingBoxSine::SlidingBoxSine(const Point3D& pos, coord_t dir, coord_t x0,
                               coord_t y0, coord_t z0, coord_t x1, coord_t y1,
                               coord_t z1, coord_t t, coord_t p)
    : SlidingBox(pos, dir, x0, y0, z0, x1, y1, z1, t, p) {}

coord_t SlidingBoxSine::getLerp(coord_t t) {
    return (sin(t / numbers::TAU<coord_t>) + 1) * 0.5;
}
}  // namespace hiemalia
