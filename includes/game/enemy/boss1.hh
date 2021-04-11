/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/boss1.hh: header file for game/enemy/boss1.cc

#ifndef M_GAME_ENEMY_BOSS1_HH
#define M_GAME_ENEMY_BOSS1_HH

#include "game/enemy.hh"

namespace hiemalia {
class EnemyBoss1 : public EnemyObject {
  public:
    EnemyBoss1(const Point3D& pos);
    bool doEnemyTick(GameWorld& w, float delta);
    void onSpawn(GameWorld& w);

  protected:
    bool onEnemyDeath(GameWorld& w, bool killedByPlayer);

  private:
    Orient3D trot{0, 0, 0};
    float fireTime_{0};
    int fireCounter_{0};
    int wall_{0};
    coord_t t_{0};
    coord_t speed_{-1};
};
};  // namespace hiemalia

#endif  // M_GAME_ENEMY_BOSS1_HH
