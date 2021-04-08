/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/box.cc: implementation of Box

#include "game/box.hh"

#include <algorithm>
#include <cmath>

#include "assets.hh"
#include "collide.hh"
#include "game/enemy.hh"
#include "game/world.hh"

namespace hiemalia {
Box::Box(const Point3D& pos, coord_t x, coord_t y, coord_t z)
    : GameObject(pos) {
    useGameModel(GameModel::BoxModel, true);
    scale = Point3D(x, y, z);
    setCollisionRadius(scale.length());
}

void Box::absorbBullets(GameWorld& w, const BulletList& list) {
    for (auto& bptr : list) {
        if (bptr->hits(*this)) {
            bptr->backtrackCuboid(pmin_, pmax_);
            bptr->impact(w, false);
        }
    }
}

void Box::absorbEnemies(GameWorld& w, const EnemyList& list) {
    for (auto& e : list) {
        if (e->hits(*this)) {
            e->hitWall(w);
        }
    }
}

bool Box::update(GameWorld& w, float delta) {
    pmin_ = pos - scale;
    pmax_ = pos + scale;
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

}  // namespace hiemalia
