/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/ebullet.hh: header file for simple enemy bullets (game/ebullet.cc)

#ifndef M_GAME_EBULLET_HH
#define M_GAME_EBULLET_HH

#include "game/bullet.hh"
#include "model.hh"

namespace hiemalia {
class EnemyBullet : public BulletObject {
  public:
    EnemyBullet(const Point3D& pos, const Point3D& v);
    virtual bool doBulletTick(GameWorld& w, float delta);
    void impact(GameWorld& w, bool enemy);
    bool firedByPlayer() const;
    float getDamage() const;
    virtual ~EnemyBullet() {}
};

class EnemyBulletLaser : public EnemyBullet {
  public:
    EnemyBulletLaser(const Point3D& pos, const Point3D& v);
};

class EnemyBulletHoming : public EnemyBullet {
  public:
    EnemyBulletHoming(const Point3D& pos, const Point3D& v);
    bool doBulletTick(GameWorld& w, float delta);
};

class EnemyBulletScalable : public EnemyBullet {
  public:
    EnemyBulletScalable(const Point3D& pos, const Point3D& v, coord_t scale,
                        int palette = 0);
};

class EnemyBulletSlideScalable : public EnemyBulletScalable {
  public:
    EnemyBulletSlideScalable(const Point3D& pos, const Point3D& v,
                             const Point3D& dst, const Point3D& nv,
                             coord_t scale);
    bool doBulletTick(GameWorld& w, float delta);

  private:
    bool cross_{false};
    Point3D target_;
    Point3D newVel_;
};

};  // namespace hiemalia

#endif  // M_GAME_EBULLET_HH
