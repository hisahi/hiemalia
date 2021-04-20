/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/pewpew.hh: header file for game/enemy/pewpew.cc

#ifndef M_GAME_ENEMY_PEWPEW_HH
#define M_GAME_ENEMY_PEWPEW_HH

#include "game/enemy.hh"

namespace hiemalia {
class EnemyPewpew : public EnemyObject {
  public:
    EnemyPewpew(const Point3D& pos, const Orient3D& rot);
    bool doEnemyTick(GameWorld& w, float delta);

  protected:
    bool onEnemyDeath(GameWorld& w, bool killedByPlayer);

  private:
    int burst_{0};
    float fireTime_{0};
    Orient3D targetRot_{0, 0, 0};
    void aim(GameWorld& w, float delta);
};
};  // namespace hiemalia

#endif  // M_GAME_ENEMY_PEWPEW_HH
