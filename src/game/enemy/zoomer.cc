/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/zoomer.cc: implementation of EnemyZoomer

#include "game/enemy/zoomer.hh"

#include "audio.hh"
#include "game/world.hh"
#include "hiemalia.hh"

namespace hiemalia {
EnemyZoomer::EnemyZoomer(const Point3D& pos) : EnemyObject(pos, 4.0F) {
    useGameModel(GameModel::EnemyZoomer);
    rot = Orient3D::atPlayer;
    vel = Point3D(0, 0, -1.5);
}

bool EnemyZoomer::doEnemyTick(GameWorld& w, float delta) {
    killPlayerOnContact(w);
    return !isOffScreen();
}

bool EnemyZoomer::onEnemyDeath(GameWorld& w, bool killedByPlayer) {
    doExplode(w);
    sendMessage(AudioMessage::playSound(SoundEffect::ExplodeSmall,
                                        pos - w.getPlayerPosition()));
    if (killedByPlayer) {
        addScore(w, 200);
        w.onEnemyKilled(*this);
    }
    return true;
}

}  // namespace hiemalia
