/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/gunboat2.hh: header file for game/enemy/gunboat2.cc

#ifndef M_GAME_ENEMY_GUNBOAT2_HH
#define M_GAME_ENEMY_GUNBOAT2_HH

#include "game/enemy.hh"

namespace hiemalia {
class EnemyGunboat2 : public EnemyObject {
  public:
    EnemyGunboat2(const Point3D& pos);
    bool doEnemyTick(GameWorld& w, float delta);

  protected:
    bool onEnemyDeath(GameWorld& w, bool killedByPlayer);

  private:
    float fireTime_{0};
};
};  // namespace hiemalia

#endif  // M_GAME_ENEMY_GUNBOAT2_HH
