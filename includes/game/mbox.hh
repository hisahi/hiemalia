/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/mbox.hh: header file for moving box object

#ifndef M_GAME_MBOX_HH
#define M_GAME_MBOX_HH

#include "assets.hh"
#include "controls.hh"
#include "game/object.hh"
#include "game/world.hh"
#include "model.hh"

namespace hiemalia {
class MovingBox : public GameObject {
  public:
    MovingBox(const Point3D& pos, int pattern, coord_t x, coord_t y, coord_t z,
              coord_t dx, coord_t dy, coord_t dz, coord_t t, coord_t p);
    bool update(GameWorld& w, float delta);

  private:
    int pattern_;
    coord_t dx_;
    coord_t dy_;
    coord_t dz_;
    coord_t x_;
    coord_t v_;
    void updateBox(GameWorld& w, float delta);
    void absorbBullets(GameWorld& w, const BulletList& list);
    void absorbEnemies(GameWorld& w, const EnemyList& list);
};
};  // namespace hiemalia

#endif  // M_GAME_MBOX_HH
