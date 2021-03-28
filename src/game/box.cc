/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/box.cc: implementation of Box

#include "game/box.hh"

#include "assets.hh"
#include "collide.hh"
#include "game/world.hh"

namespace hiemalia {
Box::Box() : GameObject() {
    model = getGameModel(GameModel::BoxModel);
    scale = ModelPoint(0.125, 0.125, 0.125);
    setCollisionRadius(0.125);
}

bool Box::update(GameWorld& w, float delta) {
    pmin = pos - scale;
    pmax = pos + scale;
    if (w.getPlayer().collideCuboid(pmin, pmax)) {
        LOG_TRACE("hi");
    }
    return !isOffScreen();
}

bool Box::collideLineInternal(const ModelPoint& p1,
                              const ModelPoint& p2) const {
    return collidesLineCuboid(p1, p2, pmin, pmax);
}

bool Box::collideCuboidInternal(const ModelPoint& c1,
                                const ModelPoint& c2) const {
    return collidesCuboidCuboid(c1, c2, pmin, pmax);
}

bool Box::collideSphereInternal(const ModelPoint& p, coord_t r2) const {
    return collidesSphereCuboid(p, r2, pmin, pmax);
}

}  // namespace hiemalia
