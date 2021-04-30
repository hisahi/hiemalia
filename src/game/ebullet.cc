/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/ebullet.cc: implementation of EnemyBulletSimple

#include "game/ebullet.hh"

#include "audio.hh"
#include "game/world.hh"
#include "hiemalia.hh"

namespace hiemalia {
EnemyBulletSimple::EnemyBulletSimple(const Point3D& pos, const Point3D& v)
    : BulletObject(pos) {
    useGameModel(GameModel::BulletEnemy);
    vel = v;
    rotvel = Orient3D(1.8, 1.2, 0.6) * 16;
}

bool EnemyBulletSimple::doBulletTick(GameWorld& w, float delta) {
    doMove(delta);
    rot += rotvel;
    if (w.isPlayerAlive() && hits(w.getPlayer())) {
        if (w.getPlayer().playerInControl())
            w.getPlayer().damage(w, getDamage(), pos);
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

EnemyBulletLaser::EnemyBulletLaser(const Point3D& pos, const Point3D& v)
    : EnemyBulletSimple(pos, v) {
    useGameModel(GameModel::BulletEnemy4);
}

EnemyBulletHoming::EnemyBulletHoming(const Point3D& pos, const Point3D& v)
    : EnemyBulletSimple(pos, v) {
    useGameModel(GameModel::BulletEnemy3);
}

static Point3D bulletAimAt(const Point3D& me, coord_t speed, const Point3D& p0,
                           const Point3D& x) {
    if (x.isZero()) return (p0 - me).normalize() * speed;
    Point3D ax = x * (1 + ((p0 - me).length() / (speed / 8)));
    Point3D p = p0 + ax;
    Point3D d = p - me;
    Point3D y = d - ax * (ax.dot(d) / ax.dot(ax));
    Point3D pointOfContact = p0 + ax * (y.length() / speed);
    return (pointOfContact - me).normalize() * speed;
}

static Point3D aimAtPlayer(GameWorld& w, const Point3D& me, float speed,
                           float lead) {
    speed *= w.difficulty().getBulletSpeedMultiplier();
    lead *= w.difficulty().getBulletLeadMultiplier();
    Point3D nonLead = bulletAimAt(me, speed, w.getPlayerPosition(),
                                  Point3D(0, 0, w.getMoveSpeed()));
    Point3D yesLead =
        lead == 0
            ? nonLead
            : bulletAimAt(
                  me, speed, w.getPlayerPosition(),
                  w.getPlayer().vel * sqrt(w.getPlayer().vel.length()) * 0.125 +
                      Point3D(0, 0, w.getMoveSpeed() + w.getMoveSpeedDelta()));
    return Point3D::lerp(nonLead, lead, yesLead);
}

bool EnemyBulletHoming::doBulletTick(GameWorld& w, float delta) {
    if (w.isPlayerAlive()) {
        Point3D ppos = w.getPlayerPosition();
        if (pos.z > ppos.z) {
            Orient3D cur = Orient3D::toPolar(vel);
            Orient3D unr = Orient3D::toPolar(
                aimAtPlayer(w, pos, static_cast<float>(vel.length()), 1.0f) -
                pos);
            Orient3D target = cur.tendTo(unr, delta * 2);
            vel = target.direction(vel.length());
        }
    }
    return EnemyBulletSimple::doBulletTick(w, delta);
}

EnemyBulletSimpleScalable::EnemyBulletSimpleScalable(const Point3D& pos,
                                                     const Point3D& v,
                                                     coord_t scale, int palette)
    : EnemyBulletSimple(pos, v) {
    this->scale *= scale;
    useGameModel(palette ? GameModel::BulletEnemy6 : GameModel::BulletEnemy2);
}

EnemyBulletSlideScalable::EnemyBulletSlideScalable(const Point3D& pos,
                                                   const Point3D& v,
                                                   const Point3D& dst,
                                                   const Point3D& nv,
                                                   coord_t scale)
    : EnemyBulletSimpleScalable(pos, v, scale), target_(dst), newVel_(nv) {}

bool EnemyBulletSlideScalable::doBulletTick(GameWorld& w, float delta) {
    bool result = EnemyBulletSimpleScalable::doBulletTick(w, delta);
    if (result && !cross_ &&
        ((vel.z > 0 && pos.z >= target_.z) ||
         (vel.z < 0 && pos.z <= target_.z))) {
        cross_ = true;
        pos.x = target_.x;
        pos.y = target_.y;
        vel = newVel_;
    }
    return result;
}

}  // namespace hiemalia
