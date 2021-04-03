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
    bool hits(const GameObject& obj) const;

   private:
    ModelPoint pmin_{0, 0, 0};
    ModelPoint pmax_{0, 0, 0};
    void absorbBullets(GameWorld& w, const BulletList& list);
};
};  // namespace hiemalia

#endif  // M_GAME_MBOX_HH
