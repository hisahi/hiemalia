/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/boss3.hh: header file for game/enemy/boss3.cc

#ifndef M_GAME_ENEMY_BOSS3_HH
#define M_GAME_ENEMY_BOSS3_HH

#include "game/ebullet.hh"
#include "game/enemy.hh"

namespace hiemalia {
class EnemyBoss3 : public EnemyObject {
  public:
    EnemyBoss3(const Point3D& pos);
    bool doEnemyTick(GameWorld& w, float delta);
    void onSpawn(GameWorld& w);
    void render(SplinterBuffer& sbuf, Renderer3D& r3d);
    bool hitEnemy(GameWorld& w, float dmg, const Point3D& pointOfContact);

  protected:
    bool onEnemyDeath(GameWorld& w, bool killedByPlayer);

  private:
    float fireTime_{0};
    coord_t speed_{0};
    coord_t eye_{0};
    coord_t a_{0};
    coord_t av_{0};
    LoadedGameModel eyeModel_;
};

class EnemyFuzzball : public EnemyObject {
  public:
    EnemyFuzzball(const Point3D& pos, coord_t v);
    bool doEnemyTick(GameWorld& w, float delta);

  protected:
    bool onEnemyDeath(GameWorld& w, bool killedByPlayer);
};
};  // namespace hiemalia

#endif  // M_GAME_ENEMY_BOSS3_HH
