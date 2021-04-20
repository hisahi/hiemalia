/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy/boss5.hh: header file for game/enemy/boss5.cc

#ifndef M_GAME_ENEMY_BOSS5_HH
#define M_GAME_ENEMY_BOSS5_HH

#include "game/ebullet.hh"
#include "game/enemy.hh"

namespace hiemalia {
class EnemyBoss5 : public EnemyObject {
  public:
    EnemyBoss5(const Point3D& pos);
    bool doEnemyTick(GameWorld& w, float delta);
    void onSpawn(GameWorld& w);
    void render(SplinterBuffer& sbuf, Renderer3D& r3d);
    bool hitEnemy(GameWorld& w, float dmg, const Point3D& pointOfContact);

  protected:
    bool onEnemyDeath(GameWorld& w, bool killedByPlayer);

  private:
    float fireTime_{0};
    int phase_{0};
    coord_t x_{0};
    coord_t a_{0};
    coord_t y_{0};
    coord_t speed_{0};
    float invul_{0};
    int burst_{0};
    Point3D dir_;
    LoadedGameModel bModel_;
};

class EnemyBulletBounce : public BulletObject {
  public:
    EnemyBulletBounce(const Point3D& pos, const Point3D& v);
    virtual bool doBulletTick(GameWorld& w, float delta);
    void impact(GameWorld& w, bool enemy);
    bool firedByPlayer() const;
    float getDamage() const;
};

};  // namespace hiemalia

#endif  // M_GAME_ENEMY_BOSS5_HH
