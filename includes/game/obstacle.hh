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
class Obstacle : public GameObject {
   public:
    Obstacle(GameModel model);
    bool update(GameWorld& w, float delta);
    virtual ~Obstacle() {}

   private:
    bool alive_{true};
    void absorbBullets(GameWorld& w, const BulletList& list);
};

class DestroyableObstacle : public GameObject, public ObjectDamageable {
   public:
    DestroyableObstacle(GameModel model, float health);
    bool update(GameWorld& w, float delta);
    virtual ~DestroyableObstacle() {}

   private:
    bool alive_{true};
    float health_;
    void onDamage(GameWorld& w, float dmg, const ModelPoint& pointOfContact);
    void onDeath(GameWorld& w);
    void absorbBullets(GameWorld& w, const BulletList& list);
};
};  // namespace hiemalia

#endif  // M_GAME_ENEMY_HH
