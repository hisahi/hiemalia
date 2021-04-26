/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/zoomer.hh: header file for game/enemy/zoomer.cc

#ifndef M_GAME_ENEMY_ZOOMER_HH
#define M_GAME_ENEMY_ZOOMER_HH

#include "game/enemy.hh"

namespace hiemalia {
class EnemyZoomer : public EnemyObject {
  public:
    EnemyZoomer(const Point3D& pos);
    bool doEnemyTick(GameWorld& w, float delta);

  protected:
    bool onEnemyDeath(GameWorld& w, bool killedByPlayer);
};
};  // namespace hiemalia

#endif  // M_GAME_ENEMY_ZOOMER_HH
