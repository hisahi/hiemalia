/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/orbiter.hh: header file for game/enemy/orbiter.cc

#ifndef M_GAME_ENEMY_ORBITER_HH
#define M_GAME_ENEMY_ORBITER_HH

#include "game/enemy.hh"

namespace hiemalia {
class EnemyOrbiter : public EnemyObject {
  public:
    EnemyOrbiter(const Point3D& pos, int pattern);
    bool doEnemyTick(GameWorld& w, float delta);

  protected:
    bool onEnemyDeath(GameWorld& w, bool killedByPlayer);

  private:
    int pattern_;
    float fireTime_{0};
    float t_{0};
    coord_t ox_{0};
    coord_t oy_{0};
    int spread_{0};
    void movePattern(GameWorld& w, float dt);
    void fireAtPlayer(GameWorld& w, float dt, coord_t fireMul);
};
};  // namespace hiemalia

#endif  // M_GAME_ENEMY_ORBITER_HH
