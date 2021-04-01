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
    virtual void renderSpecial(SplinterBuffer& sbuf, Renderer3D& r3d) {}
    virtual void onSetPosition() {}
    void setPosition(const ModelPoint& p);
    void setPosition(coord_t x, coord_t y, coord_t z);
    void move(const ModelPoint& p);
    void move(coord_t x, coord_t y, coord_t z);
    void render(SplinterBuffer& sbuf, Renderer3D& r3d);
    bool collideLine(const ModelPoint& p1, const ModelPoint& p2) const;
    bool collideCuboid(const ModelPoint& c1, const ModelPoint& c2) const;
    bool collideSphere(const ModelPoint& p, coord_t r2) const;
    bool isOffScreen() const;
    Matrix3D getObjectModelMatrix() const;

    virtual ~GameObject() {}

   protected:
    coord_t getCollisionRadiusSquared() const;
    coord_t getCollisionRadius() const;
    void setCollisionRadius(coord_t r);

   private:
    coord_t collideRadiusSquared_{0};
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
    bool damage(GameWorld& w, float damage, const ModelPoint& pointOfContact);
    float getHealth() const;

   protected:
    virtual void onDamage(GameWorld& w, float damage,
                          const ModelPoint& pointOfContact) = 0;
    virtual void onDeath(GameWorld& w) = 0;

   private:
    float health_{1};
};
};  // namespace hiemalia

#endif  // M_GAME_OBJECT_HH
