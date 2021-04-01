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
bool CheckpointScript::run(GameWorld& w) {
    if (w.isPlayerAlive()) {
        PlayerObject& p = w.getPlayer();
        if (p.shouldCatchCheckpoints() && p.pos.z >= pos.z) {
            w.setCheckpoint();
        }
        return p.pos.z < pos.z;
    }
    return true;
}

}  // namespace hiemalia
