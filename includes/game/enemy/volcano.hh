/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/volcano.hh: header file for game/enemy/volcano.cc

#ifndef M_GAME_ENEMY_VOLCANO_HH
#define M_GAME_ENEMY_VOLCANO_HH

#include "game/enemy.hh"

namespace hiemalia {
class EnemyVolcano : public EnemyObject {
  public:
    EnemyVolcano(const Point3D& pos, const Orient3D& rot,
                 const Orient3D& rotvel);
    bool doEnemyTick(GameWorld& w, float delta);

  protected:
    bool onEnemyDeath(GameWorld& w, bool killedByPlayer);

  private:
    float fireTime_{0};
    Orient3D rotvel_;
};
};  // namespace hiemalia

#endif  // M_GAME_ENEMY_VOLCANO_HH
