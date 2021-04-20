/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/blocker.hh: header file for game/enemy/blocker.cc

#ifndef M_GAME_ENEMY_BLOCKER_HH
#define M_GAME_ENEMY_BLOCKER_HH

#include "game/enemy.hh"

namespace hiemalia {
class EnemyBlocker : public EnemyObject {
  public:
    EnemyBlocker(const Point3D& pos);
    bool doEnemyTick(GameWorld& w, float delta);

  protected:
    bool onEnemyDeath(GameWorld& w, bool killedByPlayer);

  private:
    float fireTime_{0};
};

class EnemyBulletBlocker : public EnemyBulletSimple {
  public:
    EnemyBulletBlocker(const Point3D& pos, const Point3D& v);
    bool doBulletTick(GameWorld& w, float delta);
};

};  // namespace hiemalia

#endif  // M_GAME_ENEMY_BLOCKER_HH
