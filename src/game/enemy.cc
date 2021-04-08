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

EnemyObject::EnemyObject(const Point3D& pos) : EnemyObject(pos, 1.0f) {}
EnemyObject::EnemyObject(const Point3D& pos, float health)
    : GameObject(pos), ObjectDamageable(health) {}

bool EnemyObject::update(GameWorld& w, float delta) {
    if (!alive_) return false;
    bool alive = doEnemyTick(w, delta);
    if (alive) {
        for (auto& bptr : w.getPlayerBullets()) {
            if (bptr->hits(*this)) {
                Point3D c = bptr->lerp(0.5);
                float dmg = bptr->getDamage();
                if (alive_ && hitEnemy(w, dmg, c)) {
                    killedByPlayer_ = true;
                    damage(w, dmg, c);
                }
                bptr->impact(w, !alive_);
            }
        }
    }
    return alive;
}

void EnemyObject::kill(GameWorld& w) { alive_ = false; }

void EnemyObject::hitWall(GameWorld& w) {
    killedByPlayer_ = false;
    damage(w, getHealth() + 1.0f, pos);
}

void EnemyObject::onDamage(GameWorld& w, float dmg,
                           const Point3D& pointOfContact) {
    onEnemyDamage(w, dmg, pointOfContact);
}

void EnemyObject::onDeath(GameWorld& w) {
    alive_ = alive_ && !onEnemyDeath(w, killedByPlayer_);
}

void EnemyObject::doExplode(GameWorld& w) { doExplode(w, model()); }

void EnemyObject::doExplode(GameWorld& w, const Model& m) {
    w.explodeEnemy(*this, m);
}

void EnemyObject::doExplodeBoss(GameWorld& w) { doExplodeBoss(w, model()); }

void EnemyObject::doExplodeBoss(GameWorld& w, const Model& m) {
    w.explodeBoss(*this, m);
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

static Point3D bulletAimAt(const Point3D& me, coord_t speed, const Point3D& p0,
                           const Point3D& x) {
    if (x.isZero()) return (p0 - me).normalize() * speed;
    Point3D p = p0 + x;
    Point3D d = p - me;
    Point3D y = d - x * (x.dot(d) / x.dot(x));
    Point3D pointOfContact = p0 + x * (y.length() / speed);
    return (pointOfContact - me).normalize() * speed;
}

Point3D EnemyObject::aimAtPlayer(GameWorld& w, float speed, float lead) const {
    speed *= w.difficulty().getBulletSpeedMultiplier();
    lead *= w.difficulty().getBulletLeadMultiplier();
    Point3D nonLead = bulletAimAt(pos, speed, w.getPlayerPosition(),
                                  Point3D(0, 0, w.getMoveSpeed()));
    Point3D yesLead =
        lead == 0
            ? nonLead
            : bulletAimAt(
                  pos, speed, w.getPlayerPosition(),
                  w.getPlayer().vel * 0.125 +
                      Point3D(0, 0, w.getMoveSpeed() + w.getMoveSpeedDelta()));
    return Point3D::lerp(nonLead, lead, yesLead);
}

Point3D EnemyObject::getBulletVelocity(GameWorld& w, Point3D dir, float speed,
                                       float spew) const {
    speed *= w.difficulty().getBulletSpeedMultiplier();
    spew *= speed * w.difficulty().getBulletSpewMultiplier();
    return (dir + randomUnitVector() * spew).normalize() * speed;
}

}  // namespace hiemalia
