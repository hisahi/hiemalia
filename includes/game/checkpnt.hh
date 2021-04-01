/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/checkpnt.hh: header file for checkpoint script (game/checkpnt.cc)

#ifndef M_GAME_CHECKPNT_HH
#define M_GAME_CHECKPNT_HH

#include "game/script.hh"

namespace hiemalia {
class CheckpointScript : public ScriptObject {
   public:
    bool run(GameWorld& w);
};
};  // namespace hiemalia

#endif  // M_GAME_CHECKPNT_HH
