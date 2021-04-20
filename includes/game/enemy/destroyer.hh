/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/destroyer.hh: header file for game/enemy/destroyer.cc

#ifndef M_GAME_ENEMY_DESTROYER_HH
#define M_GAME_ENEMY_DESTROYER_HH

#include "game/enemy.hh"

namespace hiemalia {
class EnemyDestroyer : public EnemyObject {
  public:
    EnemyDestroyer(const Point3D& pos, int pattern);
    bool doEnemyTick(GameWorld& w, float delta);

  protected:
    bool onEnemyDeath(GameWorld& w, bool killedByPlayer);

  private:
    int pattern_;
    float fireTime_{0};
    float t_{0};
    void movePattern(GameWorld& w, float delta);
    void fireAtPlayer(GameWorld& w, float delta, coord_t fireMul);
};
};  // namespace hiemalia

#endif  // M_GAME_ENEMY_DESTROYER_HH
