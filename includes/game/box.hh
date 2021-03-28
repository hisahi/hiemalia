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
#include "model.hh"

namespace hiemalia {
class Box : public GameObject {
   public:
    Box();
    bool update(GameWorld& w, float delta);

   private:
    ModelPoint pmin{0, 0, 0};
    ModelPoint pmax{0, 0, 0};
    bool collideLineInternal(const ModelPoint& p1, const ModelPoint& p2) const;
    bool collideCuboidInternal(const ModelPoint& c1,
                               const ModelPoint& c2) const;
    bool collideSphereInternal(const ModelPoint& p, coord_t r2) const;
};
};  // namespace hiemalia

#endif  // M_GAME_BOX_HH
