/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/box.cc: implementation of Box

#include "game/box.hh"

#include <algorithm>
#include <cmath>

#include "assets.hh"
#include "audio.hh"
#include "collide.hh"
#include "game/enemy.hh"
#include "game/world.hh"
#include "hiemalia.hh"

namespace hiemalia {
Box::Box(const Point3D& pos, coord_t x, coord_t y, coord_t z)
    : GameObject(pos) {
    useGameModel(GameModel::BoxModel, true);
    scale = Point3D(x, y, z);
    setCollisionRadius(scale.length());
}

void Box::absorbBullets(GameWorld& w, const BulletList& list) {
    for (const auto& bptr : list) {
        if (bptr->hits(*this)) {
            bptr->backtrackCuboid(pos - scale, pos + scale);
            bptr->impact(w, false);
        }
    }
}

void Box::absorbEnemies(GameWorld& w, const EnemyList& list) {
    for (const auto& e : list) {
        if (e->canHitWalls() && e->hits(*this)) {
            Point3D dir = collidesCuboidPointDirection(e->pos, pos, scale);
            e->hitWall(w, dir.x, dir.y, dir.z);
        }
    }
}

bool Box::update(GameWorld& w, float delta) {
    if (w.isPlayerAlive() && w.getPlayer().hits(*this)) {
        Point3D dir =
            collidesCuboidPointDirection(w.getPlayerPosition(), pos, scale);
        w.getPlayer().wallContact(dir.x, dir.y, dir.z);
    }
    absorbEnemies(w, w.getEnemies());
    absorbBullets(w, w.getPlayerBullets());
    absorbBullets(w, w.getEnemyBullets());
    return !isOffScreen();
}

DestroyableBox::DestroyableBox(const Point3D& pos, coord_t x, coord_t y,
                               coord_t z, float health)
    : Box(pos, x, y, z), ObjectDamageable(health) {
    useGameModel(GameModel::DestroyableBoxModel, true);
}

void DestroyableBox::absorbBullets(GameWorld& w, const BulletList& list) {
    for (const auto& bptr : list) {
        if (bptr->hits(*this)) {
            bptr->backtrackCuboid(pos - scale, pos + scale);
            damage(w, bptr->getDamage(), bptr->pos);
            bptr->impact(w, false);
        }
    }
}

bool DestroyableBox::update(GameWorld& w, float delta) {
    if (w.isPlayerAlive() && w.getPlayer().hits(*this)) {
        Point3D dir =
            collidesCuboidPointDirection(w.getPlayerPosition(), pos, scale);
        w.getPlayer().wallContact(dir.x, dir.y, dir.z);
    }
    absorbEnemies(w, w.getEnemies());
    absorbBullets(w, w.getPlayerBullets());
    absorbBullets(w, w.getEnemyBullets());
    return alive_ && !isOffScreen();
}

void DestroyableBox::onDamage(GameWorld& w, float dmg,
                              const Point3D& pointOfContact) {}

void DestroyableBox::onDeath(GameWorld& w) {
    if (!alive_) return;
    SoundEffect sound;
    if (getCollisionRadius() < 1) {
        sound = SoundEffect::ExplodeSmall;
    } else if (getCollisionRadius() < 2) {
        sound = SoundEffect::ExplodeMedium;
    } else {
        sound = SoundEffect::ExplodeLarge;
    }
    sendMessage(AudioMessage::playSound(sound, pos - w.getPlayerPosition()));
    w.explodeEnemy(*this, model());
    alive_ = false;
}

}  // namespace hiemalia
