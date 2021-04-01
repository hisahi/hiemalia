/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/enemy.hh: header file for enemy objects (game/enemy.cc)

#ifndef M_GAME_ENEMY_HH
#define M_GAME_ENEMY_HH

#include "game/explode.hh"
#include "game/object.hh"
#include "game/stage.hh"
#include "model.hh"

namespace hiemalia {
class EnemyObject : public GameObject, public ObjectDamageable {
   public:
    EnemyObject();
    bool update(GameWorld& w, float delta);
    virtual bool doEnemyTick(GameWorld& w, float delta) = 0;
    virtual bool hitEnemy() = 0;
    void kill(GameWorld& w);
    virtual ~EnemyObject() {}

   protected:
    virtual void onEnemyDamage(GameWorld& w, float dmg,
                               const ModelPoint& pointOfContact) = 0;
    virtual bool onEnemyDeath(GameWorld& w, bool killedByPlayer) = 0;

   private:
    bool alive_{true};
    bool killedByPlayer_{false};
    void onDamage(GameWorld& w, float dmg, const ModelPoint& pointOfContact);
    void onDeath(GameWorld& w);
};
};  // namespace hiemalia

#endif  // M_GAME_ENEMY_HH
