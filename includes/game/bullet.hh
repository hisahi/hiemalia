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
    BulletObject();
    bool update(GameWorld& w, float delta);
    virtual bool doBulletTick(GameWorld& w, float delta) = 0;
    virtual void impact(GameWorld& w, bool enemy) = 0;
    virtual bool firedByPlayer() const = 0;
    virtual void onSetPosition();
    virtual float getDamage() const = 0;
    virtual bool hitsInternal(const GameObject& obj) const;
    ModelPoint lerp(coord_t p) const;
    void backtrackCuboid(const ModelPoint& c1, const ModelPoint& c2);
    void backtrackSphere(const ModelPoint& p, coord_t r2);
    void backtrackModel(const ModelCollision& mc, const Matrix3D mat);
    void backtrackObject(const GameObject& o);
    bool isBulletOffScreen() const;
    virtual ~BulletObject() {}

   protected:
    void kill(GameWorld& w);
    ModelPoint oldPos_{0, 0, 0};
    ModelPoint vel{0, 0, 0};
    Rotation3D rotvel{0, 0, 0};

   private:
    bool alive_{true};
    bool checkInBounds(GameWorld& w, const ModelPoint& fvel);
};
};  // namespace hiemalia

#endif  // M_GAME_BULLET_HH
