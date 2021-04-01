/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/mbox.cc: implementation of ScalableBox

#include "game/mbox.hh"

#include <algorithm>
#include <cmath>

#include "assets.hh"
#include "collide.hh"
#include "game/world.hh"

namespace hiemalia {
ScalableBox::ScalableBox(coord_t x, coord_t y, coord_t z) : GameObject() {
    model = getGameModel(GameModel::BoxModel);
    scale = ModelPoint(x, y, z);
    setCollisionRadius(std::max({x, y, z}));
}

void ScalableBox::absorbBullets(GameWorld& w, const BulletList& list) {
    for (auto& bptr : list) {
        if (bptr->hitsCuboid(pmin, pmax)) {
            bptr->backtrackCuboid(pmin, pmax);
            bptr->impact(w, false);
        }
    }
}

bool ScalableBox::update(GameWorld& w, float delta) {
    pmin = pos - scale;
    pmax = pos + scale;
    if (w.isPlayerAlive() && w.getPlayer().collideCuboid(pmin, pmax)) {
        const ModelPoint& ppos = w.getPlayer().pos;
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
    absorbBullets(w, w.getPlayerBullets());
    absorbBullets(w, w.getEnemyBullets());
    return !isOffScreen();
}

bool ScalableBox::collideLineInternal(const ModelPoint& p1,
                                      const ModelPoint& p2) const {
    return collidesLineCuboid(p1, p2, pmin, pmax);
}

bool ScalableBox::collideCuboidInternal(const ModelPoint& c1,
                                        const ModelPoint& c2) const {
    return collidesCuboidCuboid(c1, c2, pmin, pmax);
}

bool ScalableBox::collideSphereInternal(const ModelPoint& p, coord_t r2) const {
    return collidesSphereCuboid(p, r2, pmin, pmax);
}

}  // namespace hiemalia
