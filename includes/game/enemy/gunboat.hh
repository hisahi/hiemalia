/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/gunboat.hh: header file for game/enemy/gunboat.cc

#ifndef M_GAME_ENEMY_GUNBOAT_HH
#define M_GAME_ENEMY_GUNBOAT_HH

#include "game/enemy.hh"

namespace hiemalia {
class EnemyGunboat : public EnemyObject {
  public:
    EnemyGunboat(const Point3D& pos);
    bool doEnemyTick(GameWorld& w, float delta);

  protected:
    bool onEnemyDeath(GameWorld& w, bool killedByPlayer);

  private:
    float fireTime_{0};
};
};  // namespace hiemalia

#endif  // M_GAME_ENEMY_GUNBOAT_HH
