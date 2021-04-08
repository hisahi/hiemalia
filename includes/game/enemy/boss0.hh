/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/boss0.hh: header file for game/enemy/boss0.cc

#ifndef M_GAME_ENEMY_BOSS0_HH
#define M_GAME_ENEMY_BOSS0_HH

#include "game/enemy.hh"

namespace hiemalia {
class EnemyBoss0 : public EnemyObject {
  public:
    EnemyBoss0(const Point3D& pos);
    bool doEnemyTick(GameWorld& w, float delta);
    void onSpawn(GameWorld& w);

  protected:
    bool onEnemyDeath(GameWorld& w, bool killedByPlayer);

  private:
    Orient3D trot{0, 0, 0};
    float fireTime_{0};
    coord_t mv_{0};
    coord_t m_{0};
    coord_t speed_{-1};
};
};  // namespace hiemalia

#endif  // M_GAME_ENEMY_BOSS0_HH
