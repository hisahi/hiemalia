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
    EnemyObject();
    bool update(GameWorld& w, float delta);
    virtual bool doEnemyTick(GameWorld& w, float delta) = 0;
    inline virtual bool hitEnemy(GameWorld& w, float dmg,
                                 const ModelPoint& pointOfContact) {
        return true;
    }
    void kill(GameWorld& w);
    virtual ~EnemyObject() {}

   protected:
    inline virtual void onEnemyDamage(GameWorld& w, float dmg,
                                      const ModelPoint& pointOfContact){};
    inline virtual bool onEnemyDeath(GameWorld& w, bool killedByPlayer) {
        return true;
    };
    void doExplode(GameWorld& w);
    void doExplode(GameWorld& w, const Model& m);
    void addScore(GameWorld& w, unsigned int score);
    void killPlayerOnContact(GameWorld& w);

    template <typename T, typename... Ts>
    void fireBullet(GameWorld& w, const ModelPoint& turret,
                    const ModelPoint& dir, float speed, float spew,
                    Ts&&... args) const {
        w.fireEnemyBullet<T>(getObjectModelMatrix().project(turret),
                             getBulletVelocity(w, dir, speed, spew),
                             std::forward<Ts>(args)...);
    }
    ModelPoint aimAtPlayer(GameWorld& w) const;

   private:
    bool alive_{true};
    bool killedByPlayer_{false};
    void onDamage(GameWorld& w, float dmg, const ModelPoint& pointOfContact);
    void onDeath(GameWorld& w);
    ModelPoint getBulletVelocity(GameWorld& w, ModelPoint dir, float speed,
                                 float spew) const;
};
};  // namespace hiemalia

#endif  // M_GAME_ENEMY_HH
