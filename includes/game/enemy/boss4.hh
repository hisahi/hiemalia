/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/boss4.hh: header file for game/enemy/boss4.cc

#ifndef M_GAME_ENEMY_BOSS4_HH
#define M_GAME_ENEMY_BOSS4_HH

#include "game/enemy.hh"
#include "game/script.hh"

namespace hiemalia {
class EnemyBoss4 : public EnemyObject {
  public:
    EnemyBoss4(const Point3D& pos, int i);
    bool doEnemyTick(GameWorld& w, float delta);
    void onSpawn(GameWorld& w);

  protected:
    bool onEnemyDeath(GameWorld& w, bool killedByPlayer);

  private:
    Orient3D trot{0, 0, 0};
    float fireTime_{0};
    int slip_{0};
    coord_t mv_{0};
    coord_t m_{0};
    coord_t speed_{-1};
};

class Boss4Script : public ScriptObject {
  public:
    using ScriptObject::ScriptObject;
    void doScript(GameWorld& w, bool instant);
};
};  // namespace hiemalia

#endif  // M_GAME_ENEMY_BOSS4_HH
