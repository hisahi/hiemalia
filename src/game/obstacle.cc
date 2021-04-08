/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/obstacle.cc: implementation of obstacle object

#include "game/obstacle.hh"

#include "assets.hh"
#include "collide.hh"
#include "game/world.hh"

namespace hiemalia {
Obstacle::Obstacle(const Point3D& pos, const Orient3D& r, GameModel model)
    : GameObject(pos) {
    useGameModel(model);
    rot = r;
}

void Obstacle::absorbBullets(GameWorld& w, const BulletList& list) {
    for (auto& bptr : list) {
        if (hits(*bptr)) {
            bptr->backtrackObject(*this);
            bptr->impact(w, false);
        }
    }
}

bool Obstacle::update(GameWorld& w, float delta) {
    if (!alive_) return false;
    if (w.isPlayerAlive()) {
        PlayerObject& p = w.getPlayer();
        if (hits(p)) {
            p.enemyContact();
        }
    }
    absorbBullets(w, w.getPlayerBullets());
    absorbBullets(w, w.getEnemyBullets());
    return !isOffScreen();
}

DestroyableObstacle::DestroyableObstacle(const Point3D& pos, const Orient3D& r,
                                         GameModel model, float health)
    : GameObject(pos), ObjectDamageable(health) {
    useGameModel(model);
    rot = r;
}

void DestroyableObstacle::absorbBullets(GameWorld& w, const BulletList& list) {
    for (auto& bptr : list) {
        if (hits(*bptr)) {
            bptr->backtrackObject(*this);
            damage(w, bptr->getDamage(), bptr->pos);
            bptr->impact(w, false);
        }
    }
}

bool DestroyableObstacle::update(GameWorld& w, float delta) {
    if (!alive_) return false;
    if (w.isPlayerAlive()) {
        PlayerObject& p = w.getPlayer();
        if (hits(p)) {
            p.enemyContact();
        }
    }
    absorbBullets(w, w.getPlayerBullets());
    absorbBullets(w, w.getEnemyBullets());
    return !isOffScreen();
}

void DestroyableObstacle::onDamage(GameWorld& w, float dmg,
                                   const Point3D& pointOfContact) {}

void DestroyableObstacle::onDeath(GameWorld& w) {
    if (!alive_) return;
    w.explodeEnemy(*this, model());
    alive_ = false;
}

}  // namespace hiemalia
