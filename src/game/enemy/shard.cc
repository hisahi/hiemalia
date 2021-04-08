/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/shard.cc: implementation of EnemyShard

#include "game/enemy/shard.hh"

#include "game/world.hh"

namespace hiemalia {
EnemyShard::EnemyShard(const Point3D& pos) : EnemyObject(pos) {
    useGameModel(GameModel::EnemyShard);
    rot = Orient3D::atPlayer;
    vel = Point3D(0, 0, -1 / 4);
}

bool EnemyShard::doEnemyTick(GameWorld& w, float delta) {
    killPlayerOnContact(w);
    return !isOffScreen();
}

bool EnemyShard::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    doExplode(w);
    if (killedByPlayer) addScore(w, 100);
    return true;
}

}  // namespace hiemalia
