/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/bouncer.cc: implementation of EnemyBouncer

#include "game/enemy/bouncer.hh"

#include "audio.hh"
#include "game/world.hh"
#include "hiemalia.hh"
#include "random.hh"

namespace hiemalia {
static const coord_t size = 0.175;

EnemyBouncer::EnemyBouncer(const Point3D& pos, const Point3D& p2)
    : EnemyObject(pos, 6.0f), rotvel_(0, 0, 0) {
    useGameModel(GameModel::EnemyBouncer);
    rot = Orient3D::atPlayer;
    vel = p2;
    Point3D tmp = randomUnitVector() * 4;
    rotvel_ = Orient3D(tmp.x, tmp.y, tmp.z);
    // canHitWalls(true);
}

bool EnemyBouncer::doEnemyTick(GameWorld& w, float delta) {
    killPlayerOnContact(w);
    bool hit = false;
    auto region = w.getMoveRegionForZ(pos.z);
    if (pos.x - size <= region.x0)
        hit = true, vel.x = -vel.x;
    else if (pos.x + size >= region.x1)
        hit = true, vel.x = -vel.x;
    if (pos.y - size <= region.y0)
        hit = true, vel.y = -vel.y;
    else if (pos.y + size >= region.y1)
        hit = true, vel.y = -vel.y;
    if (hit) rotvel_ = -rotvel_;
    rot += rotvel_ * delta;
    return !isOffScreen2();
}

bool EnemyBouncer::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    doExplode(w);
    sendMessage(AudioMessage::playSound(SoundEffect::ExplodeSmall,
                                        pos - w.getPlayerPosition()));
    if (killedByPlayer) {
        addScore(w, 250);
        w.onEnemyKilled(*this);
    }
    return true;
}

}  // namespace hiemalia