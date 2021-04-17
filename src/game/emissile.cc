/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/emissile.cc: implementation of EnemyMissileHoming

#include "game/emissile.hh"

#include "audio.hh"
#include "game/world.hh"
#include "hiemalia.hh"

namespace hiemalia {
EnemyMissileHoming::EnemyMissileHoming(const Point3D& pos, const Point3D& v)
    : BulletObject(pos) {
    useGameModel(GameModel::BulletEnemyMissile);
    vel = v;
}

void EnemyMissileHoming::impact(GameWorld& w, bool enemy) {
    if (!enemy) {
        w.explodeBullet(*this);
    }
    kill(w);
}

bool EnemyMissileHoming::firedByPlayer() const { return false; }

float EnemyMissileHoming::getDamage() const { return 10.0f; }

bool EnemyMissileHoming::doBulletTick(GameWorld& w, float delta) {
    if (w.isPlayerAlive()) {
        Point3D ppos = w.getPlayerPosition();
        if (pos.z > ppos.z) {
            Orient3D cur = Orient3D::toPolar(vel, roll_);
            Orient3D unr = Orient3D::toPolar(ppos - pos, roll_);
            Orient3D target = cur.tendTo(unr, delta / vel.length());
            rot = target;
            vel = target.direction(vel.length() * pow<coord_t>(2, delta));
        }
    }
    roll_ = wrapAngle(roll_ + delta * 4);
    doMove(delta);
    if (w.isPlayerAlive() && hits(w.getPlayer())) {
        if (w.getPlayer().playerInControl())
            w.getPlayer().damage(w, getDamage(), pos);
        impact(w, false);
    }
    return true;
}

}  // namespace hiemalia
