/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/bouncer.hh: header file for game/enemy/bouncer.cc

#ifndef M_GAME_ENEMY_BOUNCER_HH
#define M_GAME_ENEMY_BOUNCER_HH

#include "game/enemy.hh"

namespace hiemalia {
class EnemyBouncer : public EnemyObject {
  public:
    EnemyBouncer(const Point3D& pos, const Point3D& p2);
    bool doEnemyTick(GameWorld& w, float delta);
    void hitWall(GameWorld& w, coord_t dx, coord_t dy, coord_t dz);

  protected:
    bool onEnemyDeath(GameWorld& w, bool killedByPlayer);
    Orient3D rotvel_;
    int hitFrames_{0};
    int hitClearFrames_{0};
};
};  // namespace hiemalia

#endif  // M_GAME_ENEMY_BOUNCER_HH
