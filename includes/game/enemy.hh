/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy.hh: header file for enemy objects (game/enemy.cc)

#ifndef M_GAME_ENEMY_HH
#define M_GAME_ENEMY_HH

#include "game/ebullet.hh"
#include "game/explode.hh"
#include "game/object.hh"
#include "game/stage.hh"
#include "game/world.hh"
#include "model.hh"
#include "random.hh"

namespace hiemalia {
class EnemyObject : public GameObject, public ObjectDamageable {
  public:
    explicit EnemyObject(const Point3D& pos);
    EnemyObject(const Point3D& pos, float health);
    bool update(GameWorld& w, float delta);
    virtual bool doEnemyTick(GameWorld& w, float delta) = 0;
    inline virtual bool hitEnemy(GameWorld& w, float dmg,
                                 const Point3D& pointOfContact) {
        return true;
    }
    inline bool canHitWalls() const { return canHitWalls_; }
    void kill(GameWorld& w);
    bool hitBullet(GameWorld& w, float damage, const Point3D& pointOfContact);
    virtual void hitWall(GameWorld& w, coord_t dx, coord_t dy, coord_t dz);
    virtual ~EnemyObject() {}

  protected:
    inline virtual void onEnemyDamage(GameWorld& w, float dmg,
                                      const Point3D& pointOfContact){};
    inline virtual bool onEnemyDeath(GameWorld& w, bool killedByPlayer) {
        return true;
    };
    void doExplode(GameWorld& w);
    void doExplode(GameWorld& w, const Model& m);
    void doExplodeBoss(GameWorld& w);
    void doExplodeBoss(GameWorld& w, const Model& m);
    void addScore(GameWorld& w, unsigned int score);
    void killPlayerOnContact(GameWorld& w);
    void explodeIfOutOfBounds(GameWorld& w, coord_t x0, coord_t x1, coord_t y0,
                              coord_t y1);
    inline void canHitWalls(bool flag) { canHitWalls_ = flag; }
    bool shouldBeDead() const;

    template <typename T, typename... Ts>
    void fireBullet(GameWorld& w, const Point3D& turret, const Point3D& dir,
                    float speed, float spew, Ts&&... args) const {
        w.fireEnemyBullet<T>(getObjectModelMatrix().project(turret),
                             getBulletVelocity(w, dir, speed, spew),
                             std::forward<Ts>(args)...);
    }
    template <typename T, typename... Ts>
    void fireBulletAtPlayer(GameWorld& w, const Point3D& turret, float speed,
                            float spew, float lead, Ts&&... args) const {
        w.fireEnemyBullet<T>(
            getObjectModelMatrix().project(turret),
            getBulletVelocity(w, aimAtPlayer(w, speed, lead), speed, spew),
            std::forward<Ts>(args)...);
    }
    Point3D aimAtPlayer(GameWorld& w, float speed, float lead) const;
    Point3D getBulletVelocity(GameWorld& w, Point3D dir, float speed,
                              float spew) const;

  private:
    bool alive_{true};
    bool killedByPlayer_{false};
    bool canHitWalls_{false};
    float sponge_{0.0f};
    void onDamage(GameWorld& w, float dmg, const Point3D& pointOfContact);
    void onDeath(GameWorld& w);
};
};  // namespace hiemalia

#endif  // M_GAME_ENEMY_HH
