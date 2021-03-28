/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/object.hh: header file for game objects (game/object.cc)

#ifndef M_GAME_OBJECT_HH
#define M_GAME_OBJECT_HH

#include "defs.hh"
#include "model.hh"
#include "rend3d.hh"

namespace hiemalia {
struct GameWorld;

class GameObject {
   public:
    std::shared_ptr<const Model> model;
    ModelPoint pos{0, 0, 0};
    Rotation3D rot{0, 0, 0};
    ModelPoint scale{1, 1, 1};

    virtual void onSpawn(GameWorld& w) {}
    virtual bool update(GameWorld& w, float delta) { return false; }
    void move(const ModelPoint& p);
    void move(coord_t x, coord_t y, coord_t z);
    void render(SplinterBuffer& sbuf, Renderer3D& r3d) const;
    void setCollisionRadius(coord_t r);
    bool collideLine(const ModelPoint& p1, const ModelPoint& p2) const;
    bool collideCuboid(const ModelPoint& c1, const ModelPoint& c2) const;
    bool collideSphere(const ModelPoint& p, coord_t r2) const;
    bool isOffScreen() const;

   private:
    coord_t collideRadiusSquared;
    virtual bool collideLineInternal(const ModelPoint& p1,
                                     const ModelPoint& p2) const {
        return true;
    }
    virtual bool collideCuboidInternal(const ModelPoint& c1,
                                       const ModelPoint& c2) const {
        return true;
    }
    virtual bool collideSphereInternal(const ModelPoint& p, coord_t r2) const {
        return true;
    }
};

struct ObjectDamageable {
   public:
    bool damage(float damage);

   protected:
    float health{1};
    virtual void onDamage(float damage) = 0;
    virtual void onDeath() = 0;
};
};  // namespace hiemalia

#endif  // M_GAME_OBJECT_HH
