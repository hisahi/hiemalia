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
EnemyShard::EnemyShard() {
    useGameModel(GameModel::EnemyShard);
    rot = Rotation3D::atPlayer;
}

bool EnemyShard::doEnemyTick(GameWorld& w, float delta) {
    pos.z -= delta / 32;
    killPlayerOnContact(w);
    return !isOffScreen();
}

bool EnemyShard::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    doExplode(w);
    if (killedByPlayer) addScore(w, 100);
    return true;
}

}  // namespace hiemalia
