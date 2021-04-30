/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/spider.cc: implementation of EnemySpider

#include "game/enemy/spider.hh"

#include "audio.hh"
#include "game/world.hh"
#include "hiemalia.hh"

namespace hiemalia {
EnemySpider::EnemySpider(const Point3D& pos)
    : EnemyObject(pos, 3.0f), leg_(getGameModel(GameModel::EnemySpiderLeg)) {
    useGameModel(GameModel::EnemySpider);
    height_ = leg_.model->vertices[3].y;
    rot = Orient3D::atPlayer;
    canHitWalls(true);
}

bool EnemySpider::doEnemyTick(GameWorld& w, float delta) {
    static const coord_t maxTurnSpeed = 3;
    if (pounce_ && pos.y >= w.getMoveRegionForZ(pos.z).y1 - height_ &&
        vel.y > 0) {
        pounce_ = false;
        pos.y = w.getMoveRegionForZ(pos.z).y1 - height_;
    }
    if (pounce_) {
        pos += vel * delta;
        fireTime_ += delta * 1.5f * w.difficulty().getFireRateMultiplier();
        if (w.isPlayerAlive()) {
            if (pos.z - w.getPlayerPosition().z > getCollisionRadius()) {
                while (fireTime_ >= 1) {
                    sendMessage(AudioMessage::playSound(
                        SoundEffect::EnemyFire1, pos - w.getPlayerPosition()));
                    fireBulletAtPlayer<EnemyBulletSimple>(
                        w, model().vertices[10], 0.5f, 0.0625f, 1.0f);
                    fireTime_ -= 1;
                }
            }
        }
        vel.y += delta * 1.5;
    } else {
        bool walk_ = true;
        Point3D ppos = w.getPlayerPosition();
        Point3D target = Point3D(ppos.x - pos.x, 0, ppos.z - pos.z);
        target.z += ipow<coord_t>(w.getMoveSpeed(), 4) * 0.5 * target.length();
        coord_t targetYaw = Orient3D::toPolar(target).yaw;
        coord_t angleDiff = angleDifference(targetYaw, rot.yaw);
        if (angleDiff != 0) {
            angleDiff =
                clamp(-maxTurnSpeed * delta, angleDiff, maxTurnSpeed * delta);
            rot.yaw = wrapAngle(rot.yaw + angleDiff);
        }
        pounce_ =
            target.lengthSquared() < 16 &&
            angleDifferenceAbs(targetYaw, rot.yaw) < 0.5 && walkMul_ >= 0.99 &&
            pos.z - w.getPlayerPosition().z > getCollisionRadius() * 0.5 &&
            ppos.y < pos.y;
        walk_ = walk_ && !pounce_;

        if (walk_) {
            walkMul_ = std::min(coord_t(1), walkMul_ + delta * 4.0);
            walkCycle_ = wrapAngle(walkCycle_ + walkMul_ * delta * 15);
        } else {
            walkMul_ = std::max(coord_t(0), walkMul_ - delta * 4.0);
        }
        if (angleDiff != 0) {
            walkMul_ = std::max(coord_t(0), walkMul_ - angleDiff);
        }
        pos += rot.direction(1) * walkMul_ * delta;

        if (pounce_) {
            vel = target.normalize() * 1.75;
            vel.y = clamp<coord_t>(-1.5, -sqrt(pos.y - ppos.y) * 2, -0.5);
        }
    }

    if (w.isPlayerAlive()) {
        killPlayerOnContact(w);
    }
    explodeIfOutOfBounds(w, 0.25, 0.25, 0.0625, 0);
    return !isOffScreen();
}

void EnemySpider::render(SplinterBuffer& sbuf, Renderer3D& r3d) {
    EnemyObject::render(sbuf, r3d);
    if (shouldBeDead()) return;
    static const Point3D right = Point3D(1, 1, 1);
    static coord_t m = radians<coord_t>(8);
    Orient3D r1 =
        Orient3D(radians<coord_t>(55) + walkMul_ * sin(walkCycle_) * m, 0, 0);
    Orient3D r2 =
        Orient3D(radians<coord_t>(75) - walkMul_ * sin(walkCycle_) * m, 0, 0);
    Orient3D r3 = Orient3D(
        radians<coord_t>(105) + walkMul_ * sin(walkCycle_ + 0.2) * m, 0, 0);
    Orient3D r4 = Orient3D(
        radians<coord_t>(125) - walkMul_ * sin(walkCycle_ + 0.2) * m, 0, 0);
    r3d.renderModel(sbuf, pos, rot + r1, right, *leg_.model);
    r3d.renderModel(sbuf, pos, rot + r2, right, *leg_.model);
    r3d.renderModel(sbuf, pos, rot + r3, right, *leg_.model);
    r3d.renderModel(sbuf, pos, rot + r4, right, *leg_.model);
    r3d.renderModel(sbuf, pos, rot - r1, right, *leg_.model);
    r3d.renderModel(sbuf, pos, rot - r2, right, *leg_.model);
    r3d.renderModel(sbuf, pos, rot - r3, right, *leg_.model);
    r3d.renderModel(sbuf, pos, rot - r4, right, *leg_.model);
}

void EnemySpider::onSpawn(GameWorld& w) {
    pos.y = w.getMoveRegionForZ(pos.z).y1 - height_;
}

bool EnemySpider::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    doExplode(w);
    sendMessage(AudioMessage::playSound(SoundEffect::ExplodeSmall,
                                        pos - w.getPlayerPosition()));
    if (killedByPlayer) {
        addScore(w, 700);
        w.onEnemyKilled(*this);
    }
    return true;
}

}  // namespace hiemalia
