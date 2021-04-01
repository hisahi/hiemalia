/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/mbox.hh: header file for scalable box object

#ifndef M_GAME_MBOX_HH
#define M_GAME_MBOX_HH

#include "assets.hh"
#include "controls.hh"
#include "game/object.hh"
#include "game/world.hh"
#include "model.hh"

namespace hiemalia {
class ScalableBox : public GameObject {
   public:
    ScalableBox(coord_t x, coord_t y, coord_t z);
    bool update(GameWorld& w, float delta);

   private:
    ModelPoint pmin{0, 0, 0};
    ModelPoint pmax{0, 0, 0};
    bool collideLineInternal(const ModelPoint& p1, const ModelPoint& p2) const;
    bool collideCuboidInternal(const ModelPoint& c1,
                               const ModelPoint& c2) const;
    bool collideSphereInternal(const ModelPoint& p, coord_t r2) const;
    void absorbBullets(GameWorld& w, const BulletList& list);
};
};  // namespace hiemalia

#endif  // M_GAME_MBOX_HH
