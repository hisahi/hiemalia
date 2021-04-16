/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/pbullet.cc: implementation of PlayerBullet

#include "game/pbullet.hh"

#include "audio.hh"
#include "game/enemy.hh"
#include "game/world.hh"
#include "hiemalia.hh"

namespace hiemalia {
PlayerBullet::PlayerBullet(const Point3D& pos, const Point3D& v)
    : BulletObject(pos) {
    useGameModel(GameModel::BulletPlayer);
    vel = v;
    rotvel = Orient3D(1.8, 1.2, 0.6) * 16;
}

bool PlayerBullet::doBulletTick(GameWorld& w, float delta) {
    for (auto& eptr : w.getEnemies()) {
        if (hits(*eptr)) {
            Point3D c = lerp(0.5);
            impact(w, eptr->hitBullet(w, getDamage(), c));
        }
    }
    return true;
}

void PlayerBullet::impact(GameWorld& w, bool enemy) {
    if (!enemy) {
        w.explodeBullet(*this);
    }
    kill(w);
}

bool PlayerBullet::firedByPlayer() const { return true; }

float PlayerBullet::getDamage() const { return 1.0f; }

}  // namespace hiemalia
