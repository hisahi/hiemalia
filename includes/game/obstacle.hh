/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/obstacle.hh: header file for obstacles (game/obstacle.cc)

#ifndef M_GAME_OBSTACLE_HH
#define M_GAME_OBSTACLE_HH

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
    Obstacle(const Point3D& pos, const Orient3D& rot, GameModel model);
    bool update(GameWorld& w, float delta);
    virtual ~Obstacle() {}

  protected:
    bool alive_{true};
    void absorbBullets(GameWorld& w, const BulletList& list);
    void absorbEnemies(GameWorld& w, const EnemyList& list);
};

class DestroyableObstacle : public Obstacle, public ObjectDamageable {
  public:
    DestroyableObstacle(const Point3D& pos, const Orient3D& rot,
                        GameModel model, float health);
    bool update(GameWorld& w, float delta);
    virtual ~DestroyableObstacle() {}

  private:
    void absorbBullets(GameWorld& w, const BulletList& list);
    void onDamage(GameWorld& w, float dmg, const Point3D& pointOfContact);
    void onDeath(GameWorld& w);
};
};  // namespace hiemalia

#endif  // M_GAME_OBSTACLE_HH
