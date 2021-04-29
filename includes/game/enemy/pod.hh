/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/pod.hh: header file for game/enemy/pod.cc

#ifndef M_GAME_ENEMY_POD_HH
#define M_GAME_ENEMY_POD_HH

#include "game/enemy.hh"

namespace hiemalia {
class EnemyPod : public EnemyObject {
  public:
    EnemyPod(const Point3D& pos);
    bool doEnemyTick(GameWorld& w, float delta);

  protected:
    bool onEnemyDeath(GameWorld& w, bool killedByPlayer);

  private:
    float fireTime_{0};
};
};  // namespace hiemalia

#endif  // M_GAME_ENEMY_POD_HH
