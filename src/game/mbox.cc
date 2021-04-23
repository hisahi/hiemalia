/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/mbox.cc: implementation of MovingBox

#include "game/mbox.hh"

#include <algorithm>
#include <cmath>

#include "assets.hh"
#include "collide.hh"
#include "game/enemy.hh"
#include "game/world.hh"

namespace hiemalia {
constexpr int maxPattern = 3;

MovingBox::MovingBox(const Point3D& pos, int pattern, coord_t x, coord_t y,
                     coord_t z, coord_t dx, coord_t dy, coord_t dz, coord_t t,
                     coord_t p)
    : GameObject(pos),
      pattern_(pattern % maxPattern),
      dx_(dx),
      dy_(dy),
      dz_(dz),
      x_(t),
      v_(p) {
    useGameModel(GameModel::BoxModel, true);
    scale = Point3D(x, y, z);
    setCollisionRadius(scale.length());
    switch (pattern_) {
        case 0:
            dx_ *= numbers::PI<coord_t> / 2;
            break;
        case 1:
            dy_ *= numbers::PI<coord_t> / 2;
            break;
        case 2:
            dz_ *= numbers::PI<coord_t> / 2;
            break;
        default:
            never("invalid pattern");
    }
}

void MovingBox::updateBox(GameWorld& w, float delta) {
    switch (pattern_) {
        case 0:
            pos.x += dx_ * sin(x_ * numbers::TAU<coord_t>) * delta;
            break;
        case 1:
            pos.y += dy_ * sin(x_ * numbers::TAU<coord_t>) * delta;
            break;
        case 2:
            pos.z += dz_ * sin(x_ * numbers::TAU<coord_t>) * delta;
            break;
        default:
            never("invalid pattern");
    }
}

bool MovingBox::update(GameWorld& w, float delta) {
    updateBox(w, delta);
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

void MovingBox::absorbBullets(GameWorld& w, const BulletList& list) {
    for (auto& bptr : list) {
        if (bptr->hits(*this)) {
            bptr->backtrackCuboid(pos - scale, pos + scale);
            bptr->impact(w, false);
        }
    }
}

void MovingBox::absorbEnemies(GameWorld& w, const EnemyList& list) {
    for (auto& e : list) {
        if (e->canHitWalls() && e->hits(*this)) {
            e->hitWall(w);
        }
    }
}

}  // namespace hiemalia
