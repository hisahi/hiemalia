/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/object.hh: header file for game objects (game/object.cc)

#ifndef M_GAME_OBJECT_HH
#define M_GAME_OBJECT_HH

#include "collide.hh"
#include "defs.hh"
#include "model.hh"
#include "rend3d.hh"

namespace hiemalia {
struct GameWorld;
enum class GameModel;

class GameObject {
   public:
    ModelPoint pos{0, 0, 0};
    Rotation3D rot{0, 0, 0};
    ModelPoint scale{1, 1, 1};

    virtual void onSpawn(GameWorld& w) {}
    virtual bool update(GameWorld& w, float delta) { return false; }
    virtual void renderSpecial(SplinterBuffer& sbuf, Renderer3D& r3d) {}
    virtual void onSetPosition() {}
    void setPosition(const ModelPoint& p);
    void setPosition(coord_t x, coord_t y, coord_t z);
    virtual void move(const ModelPoint& p);
    void move(coord_t x, coord_t y, coord_t z);
    void render(SplinterBuffer& sbuf, Renderer3D& r3d);
    virtual bool hits(const GameObject& obj) const;
    bool isOffScreen() const;
    Matrix3D getObjectModelMatrix() const;
    inline const Model& model() const { return *model_; }
    inline const ModelCollision& collision() const { return *collision_; }
    inline bool hasModel() const { return model_ != nullptr; }
    inline bool hasCollision() const { return collision_ != nullptr; }

    virtual ~GameObject() {}

   protected:
    coord_t getCollisionRadius() const;
    void setCollisionRadius(coord_t r);

    void setModel(std::shared_ptr<const Model>&& model);
    void setCollision(std::shared_ptr<const ModelCollision>&& collision);
    void setModel(const Model& model);
    void setCollision(const ModelCollision& collision);
    void noModel();
    void noCollision();
    void useGameModel(GameModel model, bool useCollision = true);

   private:
    coord_t collideRadius_{0};
    std::shared_ptr<const Model> modelHolder_;
    std::shared_ptr<const ModelCollision> collisionHolder_;
    const Model* model_{modelHolder_.get()};
    const ModelCollision* collision_{collisionHolder_.get()};
    virtual bool hitsInternal(const GameObject& obj) const;
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

bool collidesCuboidObject(const ModelPoint& cuboid1, const ModelPoint& cuboid2,
                          const GameObject& object);
bool collidesSphereObject(const ModelPoint& center, coord_t radius,
                          const GameObject& object);
};  // namespace hiemalia

#endif  // M_GAME_OBJECT_HH
