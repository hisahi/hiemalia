/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/ebullet.cc: implementation of EnemyBulletSimple

#include "game/ebullet.hh"

#include "game/world.hh"

namespace hiemalia {
EnemyBulletSimple::EnemyBulletSimple(const Point3D& pos, const Point3D& v)
    : BulletObject(pos) {
    useGameModel(GameModel::BulletEnemy);
    vel = v;
    rotvel = Orient3D(1.8, 1.2, 0.6) * 24;
}

bool EnemyBulletSimple::doBulletTick(GameWorld& w, float delta) {
    doMove(delta);
    if (w.isPlayerAlive() && hits(w.getPlayer())) {
        if (w.getPlayer().playerInControl()) w.getPlayer().damage(w, 1.0f, pos);
        impact(w, false);
    }
    return true;
}

void EnemyBulletSimple::impact(GameWorld& w, bool enemy) {
    if (!enemy) {
        w.explodeBullet(*this);
    }
    kill(w);
}

bool EnemyBulletSimple::firedByPlayer() const { return false; }

float EnemyBulletSimple::getDamage() const { return 1.0f; }

}  // namespace hiemalia
