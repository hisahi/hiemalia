/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/boss6.hh: header file for game/enemy/boss6.cc

#ifndef M_GAME_ENEMY_BOSS6_HH
#define M_GAME_ENEMY_BOSS6_HH

#include "game/ebullet.hh"
#include "game/enemy.hh"

namespace hiemalia {
class EnemyBoss6 : public EnemyObject {
  public:
    EnemyBoss6(const Point3D& pos);
    bool doEnemyTick(GameWorld& w, float delta);
    void onSpawn(GameWorld& w);
    void render(SplinterBuffer& sbuf, Renderer3D& r3d);
    bool hitEnemy(GameWorld& w, float dmg, const Point3D& pointOfContact);

  protected:
    bool onEnemyDeath(GameWorld& w, bool killedByPlayer);

  private:
    float fireTime_{0};
    float invul_{0};
    int phase_{0};
    coord_t x_{0};
    int rstate_{0};
    float float_{0};
    coord_t speed_{0};
    coord_t dist_{0};
    coord_t remaining_{0};
    LoadedGameModel bModel_;
};

};  // namespace hiemalia

#endif  // M_GAME_ENEMY_BOSS6_HH
