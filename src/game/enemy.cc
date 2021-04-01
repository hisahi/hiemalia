/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy.cc: implementation of base enemy object

#include "game/enemy.hh"

#include "assets.hh"
#include "collide.hh"
#include "game/world.hh"
#include "model.hh"
#include "rend3d.hh"

namespace hiemalia {
EnemyObject::EnemyObject() {}

bool EnemyObject::update(GameWorld& w, float delta) {
    if (!alive_) return false;
    bool alive = doEnemyTick(w, delta);
    if (alive) {
        for (auto& bptr : w.getPlayerBullets()) {
            if (bptr->hitsObject(*this)) {
                killedByPlayer_ = true;
                damage(w, bptr->getDamage(), bptr->lerp(0.5));
                bptr->impact(w, true);
            }
        }
    }
    return alive;
}

void EnemyObject::kill(GameWorld& w) { alive_ = false; }

void EnemyObject::onDamage(GameWorld& w, float dmg,
                           const ModelPoint& pointOfContact) {
    onEnemyDamage(w, dmg, pointOfContact);
}

void EnemyObject::onDeath(GameWorld& w) {
    alive_ = onEnemyDeath(w, killedByPlayer_);
}
}  // namespace hiemalia
