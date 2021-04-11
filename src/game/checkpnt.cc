/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/checkpnt.cc: implementation of checkpoint script object

#include "game/checkpnt.hh"

#include "game/world.hh"

namespace hiemalia {
void CheckpointScript::doScript(GameWorld& w, bool instant) {
    if (!instant) w.setCheckpoint(pos.z);
}

}  // namespace hiemalia
