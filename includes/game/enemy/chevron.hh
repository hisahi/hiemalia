/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/chevron.hh: header file for game/enemy/chevron.cc

#ifndef M_GAME_ENEMY_CHEVRON_HH
#define M_GAME_ENEMY_CHEVRON_HH

#include "game/enemy.hh"

namespace hiemalia {
class EnemyChevron : public EnemyObject {
  public:
    EnemyChevron(const Point3D& pos);
    bool doEnemyTick(GameWorld& w, float delta);

  protected:
    bool onEnemyDeath(GameWorld& w, bool killedByPlayer);

  private:
    Orient3D trot{0, 0, 0};
    bool sawPlayer_{false};
    coord_t mz_{0};
};
};  // namespace hiemalia

#endif  // M_GAME_ENEMY_CHEVRON_HH
