/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/box.hh: header file for simple box object

#ifndef M_GAME_BOX_HH
#define M_GAME_BOX_HH

#include "assets.hh"
#include "controls.hh"
#include "game/object.hh"
#include "game/world.hh"
#include "model.hh"

namespace hiemalia {
class Box : public GameObject {
  public:
    Box(const Point3D& pos, coord_t x, coord_t y, coord_t z);
    bool update(GameWorld& w, float delta);

  private:
    Point3D pmin_{0, 0, 0};
    Point3D pmax_{0, 0, 0};
    void absorbBullets(GameWorld& w, const BulletList& list);
    void absorbEnemies(GameWorld& w, const EnemyList& list);
};
};  // namespace hiemalia

#endif  // M_GAME_BOX_HH
