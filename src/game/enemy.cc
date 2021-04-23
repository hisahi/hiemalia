/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy.cc: implementation of base enemy object

#include "game/enemy.hh"

#include "assets.hh"
#include "audio.hh"
#include "collide.hh"
#include "game/world.hh"
#include "hiemalia.hh"
#include "model.hh"
#include "rend3d.hh"

namespace hiemalia {

EnemyObject::EnemyObject(const Point3D& pos) : EnemyObject(pos, 1.0f) {}
EnemyObject::EnemyObject(const Point3D& pos, float health)
    : GameObject(pos), ObjectDamageable(health) {}

bool EnemyObject::update(GameWorld& w, float delta) {
    if (sponge_ > 0) {
        for (auto& bptr : w.getPlayerBullets()) {
            if (bptr->hits(*this)) {
                bptr->impact(w, true);
            }
        }
        sponge_ -= delta;
        return sponge_ > 0;
    }

    if (!alive_) return false;
    return doEnemyTick(w, delta);
}

void EnemyObject::kill(GameWorld& w) { alive_ = false; }

bool EnemyObject::hitBullet(GameWorld& w, float dmg, const Point3D& c) {
    if (!alive_) return false;
    if (hitEnemy(w, dmg, c)) {
        killedByPlayer_ = true;
        damage(w, dmg, c);
        if (alive_)
            sendMessage(AudioMessage::playSound(SoundEffect::BulletDamage));
    } else
        sendMessage(AudioMessage::playSound(SoundEffect::BulletNoDamage));
    return alive_;
}

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
    if (!alive_) {
        noModel();
        sponge_ = 0.04f;
        vel *= 0;
    }
}

bool EnemyObject::shouldBeDead() const {
    return sponge_ > 0;
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
    Point3D ax = x * (1 + ((p0 - me).length() / 32));
    Point3D p = p0 + ax;
    Point3D d = p - me;
    Point3D y = d - ax * (ax.dot(d) / ax.dot(ax));
    Point3D pointOfContact = p0 + ax * (y.length() / speed);
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
                  w.getPlayer().vel * sqrt(w.getPlayer().vel.length()) * 0.125 +
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
