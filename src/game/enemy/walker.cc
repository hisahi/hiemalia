/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/walker.cc: implementation of EnemyWalker

#include "game/enemy/walker.hh"

#include "audio.hh"
#include "game/world.hh"
#include "hiemalia.hh"

namespace hiemalia {
static constexpr int patternCount = 1;

EnemyWalker::EnemyWalker(const Point3D& pos, int pattern)
    : EnemyObject(pos, 10.0f),
      pattern_(pattern % patternCount),
      rightAngle_(numbers::PI<coord_t>),
      leftLeg_(getGameModel(GameModel::EnemyWalkerLeg)),
      rightLeg_(getGameModel(GameModel::EnemyWalkerLeg)) {
    useGameModel(GameModel::EnemyWalker);
    exCol_.emplace_back(leftLeg_.collision, Point3D(0, 0, 0), rot,
                        Point3D(-1, 1, 1));
    exCol_.emplace_back(rightLeg_.collision, Point3D(0, 0, 0), rot,
                        Point3D(1, 1, 1));
    height_ = leftLeg_.model->vertices[4].y;
}

const std::vector<ExtraCollision>& EnemyWalker::exCollisions() const {
    return exCol_;
}

void EnemyWalker::doWalk(GameWorld& w, float delta) {
    bool walk;
    switch (pattern_) {
        case 0:
            walk = true;
            rot = Orient3D::atPlayer;
            break;
        default:
            never("invalid pattern");
    }

    if (walk) {
        pitchMul_ = std::min<coord_t>(1, pitchMul_ + delta * 2.0);
        leftAngle_ = wrapAngle(leftAngle_) + delta * 3.0;
        rightAngle_ = wrapAngle(rightAngle_) + delta * 3.0;
        leftPitch_ = sin(leftAngle_);
        rightPitch_ = sin(rightAngle_);
        pos += rot.direction(1.0 / 64 * pitchMul_) *
               std::max(cos(leftAngle_), cos(rightAngle_));
    } else if (pitchMul_ > 0) {
        pitchMul_ = std::max<coord_t>(0, pitchMul_ - delta * 2.0);
    }
}

bool EnemyWalker::doEnemyTick(GameWorld& w, float delta) {
    doWalk(w, delta);
    exCol_[0].rot.yaw = exCol_[1].rot.yaw = rot.yaw;
    exCol_[0].rot.pitch = leftPitch_ * pitchMul_ * 0.5;
    exCol_[1].rot.pitch = rightPitch_ * pitchMul_ * 0.5;
    fireTime_ += 1.25f * delta * w.difficulty().getFireRateMultiplier();
    if (w.isPlayerAlive()) {
        if (pos.z - w.getPlayerPosition().z > getCollisionRadius()) {
            while (fireTime_ >= 1) {
                sendMessage(AudioMessage::playSound(
                    SoundEffect::EnemyFire1, pos - w.getPlayerPosition()));
                fireBulletAtPlayer<EnemyBulletSimple>(w, model().vertices[0],
                                                      0.75f, 0.0625f, 1.0f);
                fireTime_ -= 1;
            }
        }
        killPlayerOnContact(w);
    }
    return !isOffScreen();
}

void EnemyWalker::render(SplinterBuffer& sbuf, Renderer3D& r3d) {
    EnemyObject::render(sbuf, r3d);
    r3d.renderModel(sbuf, pos, exCol_[0].rot, exCol_[0].scale, *leftLeg_.model);
    r3d.renderModel(sbuf, pos, exCol_[1].rot, exCol_[1].scale,
                    *rightLeg_.model);
}

void EnemyWalker::onSpawn(GameWorld& w) {
    pos.y = w.getMoveRegionForZ(pos.z).y1 - height_;
}

bool EnemyWalker::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    doExplode(w);
    w.explodeEnemy(*this, *leftLeg_.model);
    w.explodeEnemy(*this, *rightLeg_.model);
    sendMessage(AudioMessage::playSound(SoundEffect::ExplodeMedium,
                                        pos - w.getPlayerPosition()));
    if (killedByPlayer) {
        addScore(w, 600);
        w.onEnemyKilled(*this);
    }
    return true;
}

}  // namespace hiemalia
