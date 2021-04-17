/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/boss2.hh: header file for game/enemy/boss2.cc

#ifndef M_GAME_ENEMY_BOSS2_HH
#define M_GAME_ENEMY_BOSS2_HH

#include "game/enemy.hh"

namespace hiemalia {
class EnemyBoss2 : public EnemyObject {
  public:
    EnemyBoss2(const Point3D& pos);
    bool doEnemyTick(GameWorld& w, float delta);
    void onSpawn(GameWorld& w);

  protected:
    bool onEnemyDeath(GameWorld& w, bool killedByPlayer);

  private:
    float fireTime_{0};
    coord_t speed_{-1};
};
};  // namespace hiemalia

#endif  // M_GAME_ENEMY_BOSS2_HH
