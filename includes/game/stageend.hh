/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/stageend.hh: header file for checkpoint script (game/stageend.cc)

#ifndef M_GAME_STAGEEND_HH
#define M_GAME_STAGEEND_HH

#include "game/script.hh"

namespace hiemalia {
class StageEndScript : public ScriptObject {
  public:
    using ScriptObject::ScriptObject;
    void doScript(GameWorld& w, bool instant);
};
};  // namespace hiemalia

#endif  // M_GAME_STAGEEND_HH
