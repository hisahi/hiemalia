/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/stageend.cc: implementation of stage end script object

#include "game/stageend.hh"

#include "game/world.hh"

namespace hiemalia {
bool StageEndScript::run(GameWorld& w) {
    if (w.isPlayerAlive()) {
        PlayerObject& p = w.getPlayer();
        if (p.shouldCatchCheckpoints() && p.pos.z >= pos.z) {
            w.endStage();
        }
        return p.pos.z < pos.z;
    }
    return true;
}

}  // namespace hiemalia
