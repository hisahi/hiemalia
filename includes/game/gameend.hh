/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/gameend.hh: header file for game end script (game/gameend.cc)

#ifndef M_GAME_GAMEEND_HH
#define M_GAME_GAMEEND_HH

#include "game/script.hh"

namespace hiemalia {
class GameEndScript : public ScriptObject {
  public:
    using ScriptObject::ScriptObject;
    void doScript(GameWorld& w, bool instant);
};
};  // namespace hiemalia

#endif  // M_GAME_GAMEEND_HH
