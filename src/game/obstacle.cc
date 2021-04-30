/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/obstacle.cc: implementation of obstacle object

#include "game/obstacle.hh"

#include "assets.hh"
#include "audio.hh"
#include "collide.hh"
#include "game/enemy.hh"
#include "game/world.hh"
#include "hiemalia.hh"

namespace hiemalia {
Obstacle::Obstacle(const Point3D& pos, const Orient3D& r, GameModel model)
    : GameObject(pos) {
    useGameModel(model);
    rot = r;
}

void Obstacle::absorbBullets(GameWorld& w, const BulletList& list) {
    for (const auto& bptr : list) {
        if (bptr->hits(*this)) {
            bptr->backtrackObject(*this);
            bptr->impact(w, false);
        }
    }
}

void Obstacle::absorbEnemies(GameWorld& w, const EnemyList& list) {
    for (const auto& e : list) {
        if (e->canHitWalls() && e->hits(*this)) {
            e->hitWall(w, 0, 0, 0);
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
    absorbEnemies(w, w.getEnemies());
    absorbBullets(w, w.getPlayerBullets());
    absorbBullets(w, w.getEnemyBullets());
    return !isOffScreen();
}

DestroyableObstacle::DestroyableObstacle(const Point3D& pos, const Orient3D& r,
                                         GameModel model, float health)
    : Obstacle(pos, r, model), ObjectDamageable(health) {}

void DestroyableObstacle::absorbBullets(GameWorld& w, const BulletList& list) {
    for (const auto& bptr : list) {
        if (bptr->hits(*this)) {
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
    absorbEnemies(w, w.getEnemies());
    absorbBullets(w, w.getPlayerBullets());
    absorbBullets(w, w.getEnemyBullets());
    return !isOffScreen();
}

void DestroyableObstacle::onDamage(GameWorld& w, float dmg,
                                   const Point3D& pointOfContact) {}

void DestroyableObstacle::onDeath(GameWorld& w) {
    if (!alive_) return;
    SoundEffect sound;
    if (getCollisionRadius() < 0.5) {
        sound = SoundEffect::ExplodeSmall;
    } else if (getCollisionRadius() < 1.25) {
        sound = SoundEffect::ExplodeMedium;
    } else {
        sound = SoundEffect::ExplodeLarge;
    }
    sendMessage(AudioMessage::playSound(sound, pos - w.getPlayerPosition()));
    w.explodeEnemy(*this, model());
    alive_ = false;
}

}  // namespace hiemalia
