/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/boss7.hh: header file for game/enemy/boss7.cc

#ifndef M_GAME_ENEMY_BOSS7_HH
#define M_GAME_ENEMY_BOSS7_HH

#include "game/ebullet.hh"
#include "game/enemy.hh"

namespace hiemalia {
class EnemyBoss7 : public EnemyObject {
  public:
    EnemyBoss7(const Point3D& pos);
    bool doEnemyTick(GameWorld& w, float delta);
    void onSpawn(GameWorld& w);
    void render(SplinterBuffer& sbuf, Renderer3D& r3d);
    bool hitEnemy(GameWorld& w, float dmg, const Point3D& pointOfContact);

  protected:
    bool onEnemyDeath(GameWorld& w, bool killedByPlayer);

  private:
    float fireTime_{0};
    float invul_{0};
    float flip_{0};
    float fireTime2_{0};
    int dir_{-1};
    int phase_{0};
    int burst_{0};
    coord_t speed_{0};
    coord_t x_{0};
    coord_t a_{0};
    coord_t b_{0};
    coord_t c_{0};
    int lastTx_;
    int lastTy_;
    LoadedGameModel bModel_;
    LoadedGameModel cModel_;
};

};  // namespace hiemalia

#endif  // M_GAME_ENEMY_BOSS7_HH
