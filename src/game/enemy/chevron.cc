
/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/chevron.cc: implementation of EnemyChevron

#include "game/enemy/chevron.hh"

#include <cmath>

#include "game/world.hh"

namespace hiemalia {
EnemyChevron::EnemyChevron(const Point3D& pos) : EnemyObject(pos, 10.0f) {
    useGameModel(GameModel::EnemyChevron);
    rot = Orient3D::atPlayer;
}

bool EnemyChevron::doEnemyTick(GameWorld& w, float delta) {
    if (sawPlayer_) {
        if (w.isPlayerAlive()) {
            Point3D d = w.getPlayerPosition() - pos;
            coord_t m = w.getMoveSpeed() * 2;
            d.x *= m;
            d.y *= m;
            d = d.normalize();
            Point3D v = vel.normalize();
            coord_t vl = vel.length();
            vel = Point3D::lerp(v, clamp<float>(0, 4 * delta, 1), d);
            vel = vl * d.dot(v) * vel.normalize();
            vel.z = std::min(mz_, vel.z);
        }
        trot.roll = vel.x;
        vel *= std::pow(2, delta);
        if (rot.roll < trot.roll) {
            rot.roll = std::min(trot.roll, rot.roll + 0.5 * delta);
        } else if (rot.roll > trot.roll) {
            rot.roll = std::max(trot.roll, rot.roll - 0.5 * delta);
        }
        mz_ = std::min(mz_, vel.z * 0.75);
    } else if (w.isPlayerAlive() &&
               (pos - w.getPlayerPosition()).z < stageSpawnDistance - 0.5) {
        sawPlayer_ = true;
        vel = ((w.getPlayerPosition() - pos) - Point3D(0, 0, 2)).normalize();
    } else {
        doMove(delta, {0, 0, -1 / 128});
    }
    killPlayerOnContact(w);
    coord_t xr = lerp(0.2 * 0.375, cos(rot.roll), 1 * 0.375);
    coord_t yr = lerp(0.2 * 0.375, sin(rot.roll), 1 * 0.375);
    if (!isInRegion(w, xr, xr, yr, yr)) {
        hitWall(w);
    }
    return !isOffScreen();
}

bool EnemyChevron::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    doExplode(w);
    if (killedByPlayer) addScore(w, 200);
    return true;
}
};  // namespace hiemalia
