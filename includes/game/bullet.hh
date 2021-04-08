/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/bullet.hh: header file for bullet objects (game/bullet.cc)

#ifndef M_GAME_BULLET_HH
#define M_GAME_BULLET_HH

#include "game/explode.hh"
#include "game/object.hh"
#include "game/stage.hh"
#include "model.hh"

namespace hiemalia {
class BulletObject : public GameObject {
  public:
    BulletObject(const Point3D& pos);
    bool update(GameWorld& w, float delta);
    virtual bool doBulletTick(GameWorld& w, float delta) = 0;
    virtual void impact(GameWorld& w, bool enemy) = 0;
    virtual bool firedByPlayer() const = 0;
    virtual void onMove(const Point3D& newPos);
    virtual float getDamage() const = 0;
    virtual bool hitsSweep(const GameObject& obj) const;
    virtual bool hitsInternal(const GameObject& obj) const;
    Point3D lerp(coord_t p) const;
    void backtrackCuboid(const Point3D& c1, const Point3D& c2);
    void backtrackSphere(const Point3D& p, coord_t r2);
    void backtrackModel(const ModelCollision& mc, const Matrix3D mat);
    void backtrackObject(const GameObject& o);
    bool isBulletOffScreen() const;
    virtual ~BulletObject() {}

  protected:
    void kill(GameWorld& w);
    Orient3D rotvel{0, 0, 0};

  private:
    bool alive_{true};
    bool checkInBounds(GameWorld& w);
};
};  // namespace hiemalia

#endif  // M_GAME_BULLET_HH
