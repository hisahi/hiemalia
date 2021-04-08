/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/shard.hh: header file for game/enemy/shard.cc

#ifndef M_GAME_ENEMY_SHARD_HH
#define M_GAME_ENEMY_SHARD_HH

#include "game/enemy.hh"

namespace hiemalia {
class EnemyShard : public EnemyObject {
  public:
    EnemyShard(const Point3D& pos);
    bool doEnemyTick(GameWorld& w, float delta);

  protected:
    bool onEnemyDeath(GameWorld& w, bool killedByPlayer);
};
};  // namespace hiemalia

#endif  // M_GAME_ENEMY_SHARD_HH
