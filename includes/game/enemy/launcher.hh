/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/launcher.hh: header file for game/enemy/launcher.cc

#ifndef M_GAME_ENEMY_LAUNCHER_HH
#define M_GAME_ENEMY_LAUNCHER_HH

#include "game/enemy.hh"

namespace hiemalia {
class EnemyLauncher : public EnemyObject {
  public:
    EnemyLauncher(const Point3D& pos, const Orient3D& rot);
    bool doEnemyTick(GameWorld& w, float delta);

  protected:
    bool onEnemyDeath(GameWorld& w, bool killedByPlayer);

  private:
    float fireTime_{0};
};
};  // namespace hiemalia

#endif  // M_GAME_ENEMY_LAUNCHER_HH
