/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/sbox.hh: header file for sliding box object

#ifndef M_GAME_SBOX_HH
#define M_GAME_SBOX_HH

#include "assets.hh"
#include "controls.hh"
#include "game/object.hh"
#include "game/world.hh"
#include "model.hh"

namespace hiemalia {
class SlidingBox : public GameObject {
  public:
    SlidingBox(const Point3D& pos, coord_t dir, coord_t x0, coord_t y0,
               coord_t z0, coord_t x1, coord_t y1, coord_t z1, coord_t t,
               coord_t p);
    bool update(GameWorld& w, float delta);

  private:
    coord_t x_;
    coord_t v_;
    Point3D p0_{0, 0, 0};
    Point3D p1_{0, 0, 0};
    Point3D pos_;
    Point3D* pmin_;
    Point3D* pmax_;
    bool isY_;
    std::shared_ptr<Model> model_;
    std::shared_ptr<ModelCollision> collision_;
    virtual coord_t getLerp(coord_t t);
    void updateBox();
    void absorbBullets(GameWorld& w, const BulletList& list);
    void absorbEnemies(GameWorld& w, const EnemyList& list);
};

class SlidingBoxSine : public SlidingBox {
  public:
    SlidingBoxSine(const Point3D& pos, coord_t dir, coord_t x0, coord_t y0,
                   coord_t z0, coord_t x1, coord_t y1, coord_t z1, coord_t t,
                   coord_t p);

  private:
    coord_t getLerp(coord_t t);
};
};  // namespace hiemalia

#endif  // M_GAME_SBOX_HH
