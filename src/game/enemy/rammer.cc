
/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/rammer.cc: implementation of EnemyRammer

#include "game/enemy/rammer.hh"

#include <cmath>

#include "audio.hh"
#include "game/world.hh"
#include "hiemalia.hh"

namespace hiemalia {
static const coord_t width = 0.25 * 0.4375;
static const coord_t height = 0.25 * 0.25;

EnemyRammer::EnemyRammer(const Point3D& pos) : EnemyObject(pos, 10.0f) {
    useGameModel(GameModel::EnemyRammer);
    rot = Orient3D::atPlayer;
    canHitWalls(true);
}

bool EnemyRammer::doEnemyTick(GameWorld& w, float delta) {
    switch (state_) {
        case 0: {
            auto r = w.getMoveRegionForZ(pos.z);
            auto ppos = w.getPlayerPosition();
            coord_t tx = clamp(r.x0 + width / 2, ppos.x, r.x1 - width / 2);
            coord_t ty = clamp(r.y0 + height / 2, ppos.y, r.y1 - height / 2);
            Point3D op = pos;
            Point3D np = Point3D(tx, ty, pos.z);
            coord_t v = 0.175 * delta;
            Point3D dp = np - op;
            if (dp.length() > v) dp = dp.normalize() * v;

            pos += dp;

            float_ += 4.0f * delta;
            float_ = std::max(
                0.0f, float_ - static_cast<float>(delta * (op - pos).length()));

            if (float_ >= 1 && abs(pos.x - tx) < 0.1 && abs(pos.y - ty) < 0.1) {
                dist_ = pos.z - ppos.z;
                remaining_ = dist_;
                state_ = 1;
            }
            break;
        }
        case 1: {
            coord_t speed =
                4 * delta * w.difficulty().getEnemySpeedMultiplier();
            pos.z -= std::min(remaining_, speed);
            remaining_ -= speed;
            if (remaining_ < 0) {
                remaining_ = dist_;
                state_ = 2;
            }
            break;
        }
        case 2: {
            coord_t speed =
                1.75 * delta * w.difficulty().getEnemySpeedMultiplier();
            pos.z += std::min(remaining_, speed);
            remaining_ -= speed;
            if (remaining_ < 0) {
                remaining_ = dist_;
                state_ = 0;
            }
            break;
        }
    }
    pos.z += w.getMoveSpeed() * delta * 0.75;

    killPlayerOnContact(w);
    if (!isInRegion(w, width, width, height, height)) {
        hitWall(w);
    }
    return !isOffScreen();
}

bool EnemyRammer::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    doExplode(w);
    sendMessage(AudioMessage::playSound(SoundEffect::ExplodeMedium,
                                        pos - w.getPlayerPosition()));
    if (killedByPlayer) {
        addScore(w, 500);
        w.onEnemyKilled(*this);
    }
    return true;
}
};  // namespace hiemalia
