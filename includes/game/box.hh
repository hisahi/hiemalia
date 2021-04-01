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
#include "mbox.hh"
#include "model.hh"

namespace hiemalia {
class Box : public ScalableBox {
   public:
    Box();
};
};  // namespace hiemalia

#endif  // M_GAME_BOX_HH
