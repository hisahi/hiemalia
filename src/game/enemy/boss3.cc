/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/boss3.cc: implementation of EnemyBoss3

#include "game/enemy/boss3.hh"

#include "game/gamemsg.hh"
#include "game/world.hh"
#include "hiemalia.hh"

namespace hiemalia {

EnemyBoss3::EnemyBoss3(const Point3D& pos) : EnemyObject(pos, 40.0f) {
    useGameModel(GameModel::EnemyBoss3);
    fireTime_ =
        getRandomPool().random(std::uniform_real_distribution<float>(0, 1));
    rot = Orient3D::atPlayer;
    eyeModel_ = getGameModel(GameModel::EnemyBoss3B);
    av_ = 0.5;
}

bool EnemyBoss3::hitEnemy(GameWorld& w, float dmg,
                          const Point3D& pointOfContact) {
    return eye_ > 0;
}

void EnemyBoss3::onSpawn(GameWorld& w) {
    speed_ = w.pushBoss(
        {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17}, 0.333);
}

void EnemyBoss3::render(SplinterBuffer& sbuf, Renderer3D& r3d) {
    EnemyObject::render(sbuf, r3d);
    if (eye_ > 0) {
        coord_t s = std::min<coord_t>({1, eye_, 3 - eye_});
        r3d.renderModel(sbuf, pos, rot, Point3D(s, s, 1), *eyeModel_.model);
    }
}

bool EnemyBoss3::doEnemyTick(GameWorld& w, float delta) {
    if (pos.z - getCollisionRadius() > stageSpawnDistance) return true;
    coord_t tx = w.getPlayerPosition().x * 0.5;
    coord_t ty = w.getPlayerPosition().y * 0.5;
    coord_t tz = w.getPlayerPosition().z + 2.75;
    Point3D op = pos;
    Point3D np = Point3D(tx, ty, pos.z);
    coord_t v = 0.0625 * delta;
    Point3D dp = np - op;
    if (dp.length() > v) dp = dp.normalize() * v;
    if (pos.z < tz)
        pos.z = std::min(tz, pos.z + delta * 0.75);
    else if (pos.z > tz)
        pos.z = std::max(tz, pos.z - delta * 0.75);
    killPlayerOnContact(w);
    if (eye_ > 0) {
        pos += dp;
        if (eye_ <= 2 && eye_ + delta > 2) {
            sendMessage(AudioMessage::playSound(SoundEffect::EnemyFire4,
                                                pos - w.getPlayerPosition()));
            w.spawnEnemy<EnemyFuzzball, coord_t>(
                pos, 0.375 * w.difficulty().getEnemySpeedMultiplier());
        }
        eye_ += delta;
        if (eye_ >= 3) {
            eye_ = 0;
            a_ = 0;
            av_ = 0.5;
        }
    } else {
        if (w.isPlayerAlive() &&
            pos.z - w.getPlayerPosition().z > getCollisionRadius() * 0.5) {
            coord_t s = sin(a_);
            coord_t c = cos(a_);
            while (fireTime_ >= 1) {
                sendMessage(AudioMessage::playSound(
                    SoundEffect::EnemyFire2, pos - w.getPlayerPosition()));
                for (int x = -5; x <= 5; ++x) {
                    fireBullet<EnemyBulletSimpleScalable>(
                        w, model().vertices[0], Point3D(x * s, x * c, -10),
                        2.5f, 0.0f, 0.5f);
                }
                fireTime_ -= 1;
            }
            fireTime_ += 5 * delta;
            a_ += av_ * delta;
            av_ += 0.15 * delta;
            if (a_ >= 2.75 * numbers::TAU<coord_t>) {
                eye_ += 0.01;
            }
        }
    }
    return true;
}

bool EnemyBoss3::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    doExplodeBoss(w);
    sendMessage(AudioMessage::playSound(SoundEffect::ExplodeLarge,
                                        pos - w.getPlayerPosition()));
    sendMessage(GameMessage::shakeCamera(0.0625));
    if (speed_ >= 0) w.popBoss(speed_);
    if (killedByPlayer) {
        addScore(w, 4000);
        w.onEnemyKilled(*this);
    }
    return true;
}

EnemyFuzzball::EnemyFuzzball(const Point3D& pos, coord_t v)
    : EnemyObject(pos, 20.0f) {
    useGameModel(GameModel::EnemyFuzzball);
    vel = Point3D(0, 0, -v);
    canHitWalls(true);
}

bool EnemyFuzzball::doEnemyTick(GameWorld& w, float delta) {
    if (w.isPlayerAlive()) {
        Point3D ppos = w.getPlayerPosition();
        if (pos.z > ppos.z) {
            Orient3D cur = Orient3D::toPolar(vel);
            Orient3D unr = Orient3D::toPolar(ppos - pos);
            Orient3D target = cur.tendTo(unr, delta * 2);
            vel = target.direction(vel.length());
        }
    }
    rot += Orient3D(33, 37, 43) * delta;
    killPlayerOnContact(w);
    explodeIfOutOfBounds(w, 0.25, 0.25, 0.25, 0.25);
    return !isOffScreen();
}

bool EnemyFuzzball::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    doExplode(w);
    sendMessage(AudioMessage::playSound(SoundEffect::ExplodeSmall,
                                        pos - w.getPlayerPosition()));
    return true;
}

}  // namespace hiemalia
