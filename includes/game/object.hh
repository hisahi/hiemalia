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

namespace hiemalia {
struct GameWorld;
enum class GameModel;

class GameObject {
  public:
    Point3D pos;
    Orient3D rot{0, 0, 0};
    Point3D vel{0, 0, 0};
    Point3D scale{1, 1, 1};

    virtual inline void onSpawn(GameWorld& w) {}
    bool tick(GameWorld& w, float delta);
    virtual inline void onMove(const Point3D& newPos) {}
    virtual inline void instant(GameWorld& w) {}
    void setPosition(const Point3D& p);
    void setPosition(coord_t x, coord_t y, coord_t z);
    void move(const Point3D& pos);
    void move(coord_t x, coord_t y, coord_t z);
    virtual void render(SplinterBuffer& sbuf, Renderer3D& r3d);
    virtual bool hits(const GameObject& obj) const;
    bool isOffScreen() const;
    bool isInRegion(GameWorld& w, coord_t extentLeft, coord_t extentRight,
                    coord_t extentTop, coord_t extentBottom) const;
    Matrix3D getObjectModelMatrix() const;
    inline const Model& model() const { return *model_; }
    inline const ModelCollision& collision() const { return *collision_; }
    inline bool hasModel() const noexcept { return model_ != nullptr; }
    inline bool hasCollision() const noexcept { return collision_ != nullptr; }

    GameObject(const Point3D& pos);
    virtual ~GameObject() noexcept {}

  protected:
    Point3D oldPos_{0, 0, 0};

    coord_t getCollisionRadius() const;
    void setCollisionRadius(coord_t r);

    virtual inline bool update(GameWorld& w, float delta) { return false; }
    void setModel(std::shared_ptr<const Model>&& model);
    void setCollision(std::shared_ptr<const ModelCollision>&& collision);
    void setModel(const Model& model);
    void setCollision(const ModelCollision& collision);
    void noModel();
    void noCollision();
    void useGameModel(GameModel model, bool useCollision = true);
    void doMove(float delta);
    void doMove(float delta, const Point3D& v);
    virtual bool hitsSweep(const GameObject& obj) const;
    virtual bool hitsInternal(const GameObject& obj) const;

  private:
    coord_t collideRadius_{0};
    std::shared_ptr<const Model> modelHolder_;
    std::shared_ptr<const ModelCollision> collisionHolder_;
    const Model* model_{modelHolder_.get()};
    const ModelCollision* collision_{collisionHolder_.get()};
};

struct ObjectDamageable {
  public:
    inline ObjectDamageable() : health_(1) {}
    inline explicit ObjectDamageable(float health) : health_(health) {}
    bool damage(GameWorld& w, float damage, const Point3D& pointOfContact);
    float getHealth() const;

  protected:
    virtual void onDamage(GameWorld& w, float damage,
                          const Point3D& pointOfContact) = 0;
    virtual void onDeath(GameWorld& w) = 0;

  private:
    float health_;
};

bool collidesCuboidObject(const Point3D& cuboid1, const Point3D& cuboid2,
                          const GameObject& object);
bool collidesSphereObject(const Point3D& center, coord_t radius,
                          const GameObject& object);
bool collidesSweepSphereObject(const Point3D& c1, const Point3D& c2, coord_t r,
                               const GameObject& object);
};  // namespace hiemalia

#endif  // M_GAME_OBJECT_HH
