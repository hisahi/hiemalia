/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/boss5.cc: implementation of EnemyBoss5

#include "game/enemy/boss5.hh"

#include "game/gamemsg.hh"
#include "game/world.hh"
#include "hiemalia.hh"

namespace hiemalia {

EnemyBoss5::EnemyBoss5(const Point3D& pos)
    : EnemyObject(pos, 60.0f), dir_(0, 0, 0) {
    useGameModel(GameModel::EnemyBoss5);
    fireTime_ =
        getRandomPool().random(std::uniform_real_distribution<float>(0, 1));
    rot = Orient3D::atPlayer;
    bModel_ = getGameModel(GameModel::EnemyBoss5B);
}

bool EnemyBoss5::hitEnemy(GameWorld& w, float dmg,
                          const Point3D& pointOfContact) {
    return invul_ == 0;
}

void EnemyBoss5::onSpawn(GameWorld& w) {
    speed_ = w.pushBoss({19, 20, 21, 22, 23, 24, 25, 26}, 0.333);
}

void EnemyBoss5::render(SplinterBuffer& sbuf, Renderer3D& r3d) {
    EnemyObject::render(sbuf, r3d);
    if (phase_ == 0) r3d.renderModel(sbuf, pos, rot, scale, *bModel_.model);
}

bool EnemyBoss5::doEnemyTick(GameWorld& w, float delta) {
    if (pos.z - getCollisionRadius() > stageSpawnDistance) return true;
    if (invul_ > 0) invul_ = std::max<float>(0, invul_ - delta);
    coord_t tx, ty, v;
    if (phase_ == 1 && invul_ == 0) {
        tx = sin(y_) * 1;
        ty = cos(y_ * 5) * 0.625;
        v = 0.375 * delta;
        y_ = wrapAngle(y_ + delta);
        Point3D targetVel = Point3D(tx - pos.x, ty - pos.y, 0);
        vel = Point3D::lerp(vel, 0.25, targetVel);
        pos += vel * delta;
    } else {
        if (invul_ > 0)
            tx = ty = 0, v = 0.125 * delta;
        else {
            tx = sin(y_) * 0.5;
            ty = 0;
            v = 0.25 * delta;
            y_ = wrapAngle(y_ + 2 * delta);
        }
        Point3D op = pos;
        Point3D np = Point3D(tx, ty, pos.z);
        Point3D dp = np - op;
        if (dp.length() > v) dp = dp.normalize() * v;
        pos += dp;
    }
    coord_t tz = w.getPlayerPosition().z + 2.75;
    if (pos.z < tz)
        pos.z = std::min(tz, pos.z + delta * 0.75);
    else if (pos.z > tz)
        pos.z = std::max(tz, pos.z - delta * 0.75);
    killPlayerOnContact(w);
    if (invul_ > 0 || !w.isPlayerAlive()) return true;
    switch (phase_) {
        case 0:
            x_ += delta * 20;
            while (x_ > 1) {
                sendMessage(AudioMessage::playSound(
                    SoundEffect::EnemyFire1, pos - w.getPlayerPosition()));
                coord_t z =
                    lerp<coord_t>(4.5, unlerp(0.0f, getHealth(), 60.0f), 2.5);
                coord_t s = sin(a_), c = cos(a_);
                a_ += numbers::PI<coord_t> / 16;
                fireBullet<EnemyBulletSimpleScalable>(w, model().vertices[0],
                                                      Point3D(s, c, -z), 2.5f,
                                                      0.0f, 0.5f);
                fireBullet<EnemyBulletSimpleScalable>(w, model().vertices[0],
                                                      Point3D(c, -s, -z), 2.5f,
                                                      0.0f, 0.5f);
                fireBullet<EnemyBulletSimpleScalable>(w, model().vertices[0],
                                                      Point3D(-s, -c, -z), 2.5f,
                                                      0.0f, 0.5f);
                fireBullet<EnemyBulletSimpleScalable>(w, model().vertices[0],
                                                      Point3D(-c, s, -z), 2.5f,
                                                      0.0f, 0.5f);
                x_ -= 1;
            }
            fireTime_ = fireTime_ + (burst_ ? 10.0f : 0.75f) * delta *
                                        w.difficulty().getFireRateMultiplier();
            while (fireTime_ > 1) {
                if (burst_)
                    --burst_;
                else {
                    burst_ = 10;
                    sendMessage(AudioMessage::playSound(
                        SoundEffect::EnemyFire6, pos - w.getPlayerPosition()));
                    dir_ = aimAtPlayer(w, 3.5f, 1.0f);
                }
                fireBullet<EnemyBulletLaser>(w, model().vertices[0], dir_, 3.5f,
                                             0.0f);
                fireTime_ -= 1;
            }
            break;
        case 1:
            fireTime_ = fireTime_ +
                        5.0f * delta * w.difficulty().getFireRateMultiplier();
            while (fireTime_ > 1) {
                sendMessage(AudioMessage::playSound(
                    SoundEffect::EnemyFire4, pos - w.getPlayerPosition()));
                for (int t = -1; t <= 1; ++t)
                    fireBullet<EnemyBulletBounce>(
                        w, model().vertices[0],
                        Point3D(sin(a_ + t * 0.125), cos(a_ + t * 0.125),
                                -(sin(x_) * 0.25) - 0.375),
                        0.375f, 0.0f);
                fireTime_ -= 1;
            }
            a_ = wrapAngle(a_ + delta * 0.89);
            x_ = wrapAngle(x_ + delta * 1.77);
            break;
    }
    return true;
}

bool EnemyBoss5::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    if (phase_ == 0) {
        if (killedByPlayer) {
            addScore(w, 1000);
        }
        sendMessage(AudioMessage::playSound(SoundEffect::ExplodeMedium,
                                            pos - w.getPlayerPosition()));
        w.explodeEnemy(*this, *bModel_.model);
        ++phase_;
        invul_ = 2.5f;
        setHealth(60.0f);
        return false;
    }
    sendMessage(AudioMessage::playSound(SoundEffect::ExplodeLarge,
                                        pos - w.getPlayerPosition()));
    doExplodeBoss(w);
    sendMessage(GameMessage::shakeCamera(0.0625));
    if (speed_ >= 0) w.popBoss(speed_);
    if (killedByPlayer) {
        addScore(w, 2000);
        w.onEnemyKilled(*this);
    }
    return true;
}

EnemyBulletBounce::EnemyBulletBounce(const Point3D& pos, const Point3D& v)
    : BulletObject(pos) {
    useGameModel(GameModel::BulletEnemy5);
    vel = v;
    rotvel = Orient3D(1.8, 1.2, 0.6) * 16;
}

bool EnemyBulletBounce::doBulletTick(GameWorld& w, float delta) {
    doMove(delta);
    rot += rotvel;
    if (w.isPlayerAlive() && hits(w.getPlayer())) {
        if (w.getPlayer().playerInControl())
            w.getPlayer().damage(w, getDamage(), pos);
        impact(w, false);
    }
    auto region = w.getMoveRegionForZ(pos.z);
    if (pos.x + vel.x <= region.x0)
        vel.x = -vel.x;
    else if (pos.x + vel.x >= region.x1)
        vel.x = -vel.x;
    if (pos.y + vel.y <= region.y0)
        vel.y = -vel.y;
    else if (pos.y + vel.y >= region.y1)
        vel.y = -vel.y;
    return true;
}

void EnemyBulletBounce::impact(GameWorld& w, bool enemy) {
    if (!enemy) {
        w.explodeBullet(*this);
    }
    kill(w);
}

bool EnemyBulletBounce::firedByPlayer() const { return false; }

float EnemyBulletBounce::getDamage() const { return 1.0f; }

}  // namespace hiemalia
