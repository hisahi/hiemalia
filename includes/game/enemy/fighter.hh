/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/fighter.hh: header file for game/enemy/fighter.cc

#ifndef M_GAME_ENEMY_FIGHTER_HH
#define M_GAME_ENEMY_FIGHTER_HH

#include "game/enemy.hh"

namespace hiemalia {
class EnemyFighter : public EnemyObject {
  public:
    EnemyFighter(const Point3D& pos, int pattern);
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

#endif  // M_GAME_ENEMY_FIGHTER_HH
