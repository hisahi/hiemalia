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
    useGameModel(GameModel::BoxModel, false);
    scale = ModelPoint(x, y, z);
    noCollision();  // because we override hits
}

bool ScalableBox::hits(const GameObject& obj) const {
    return collidesCuboidObject(pmin_, pmax_, obj);
}

void ScalableBox::absorbBullets(GameWorld& w, const BulletList& list) {
    for (auto& bptr : list) {
        if (hits(*bptr)) {
            bptr->backtrackCuboid(pmin_, pmax_);
            bptr->impact(w, false);
        }
    }
}

bool ScalableBox::update(GameWorld& w, float delta) {
    pmin_ = pos - scale;
    pmax_ = pos + scale;
    if (w.isPlayerAlive() && hits(w.getPlayer())) {
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

}  // namespace hiemalia
