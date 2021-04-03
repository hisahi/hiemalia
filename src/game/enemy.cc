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
            if (bptr->hits(*this)) {
                ModelPoint c = bptr->lerp(0.5);
                float dmg = bptr->getDamage();
                if (hitEnemy(w, dmg, c)) {
                    killedByPlayer_ = true;
                    damage(w, dmg, c);
                }
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
    alive_ = alive_ && !onEnemyDeath(w, killedByPlayer_);
}

void EnemyObject::doExplode(GameWorld& w) { doExplode(w, model()); }

void EnemyObject::doExplode(GameWorld& w, const Model& m) {
    w.explodeEnemy(*this, m);
}

void EnemyObject::addScore(GameWorld& w, unsigned int score) {
    w.addScore(score);
}

void EnemyObject::killPlayerOnContact(GameWorld& w) {
    if (w.isPlayerAlive()) {
        PlayerObject& p = w.getPlayer();
        if (hits(p)) {
            p.enemyContact();
        }
    }
}

ModelPoint EnemyObject::aimAtPlayer(GameWorld& w) const {
    return w.getPlayerPosition() - pos;
}

ModelPoint EnemyObject::getBulletVelocity(GameWorld& w, ModelPoint dir,
                                          float speed, float spew) const {
    speed *= w.difficulty().getBulletSpeedMultiplier();
    spew *= speed * w.difficulty().getBulletSpewMultiplier();
    return (dir + randomUnitVector() * spew).normalize() * speed;
}

}  // namespace hiemalia
