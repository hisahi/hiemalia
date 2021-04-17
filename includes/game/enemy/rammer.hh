/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/rammer.hh: header file for game/enemy/rammer.cc

#ifndef M_GAME_ENEMY_RAMMER_HH
#define M_GAME_ENEMY_RAMMER_HH

#include "game/enemy.hh"

namespace hiemalia {
class EnemyRammer : public EnemyObject {
  public:
    EnemyRammer(const Point3D& pos);
    bool doEnemyTick(GameWorld& w, float delta);

  protected:
    bool onEnemyDeath(GameWorld& w, bool killedByPlayer);

  private:
    int state_{0};
    coord_t dist_{0};
    coord_t remaining_{0};
    float float_{0};
};
};  // namespace hiemalia

#endif  // M_GAME_ENEMY_RAMMER_HH
