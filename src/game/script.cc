/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/script.cc: implementation of script objects

#include "game/script.hh"

#include "game/player.hh"
#include "game/world.hh"

namespace hiemalia {
ScriptObject::ScriptObject(const Point3D& pos) : GameObject(pos) {}

bool ScriptObject::update(GameWorld& w, float delta) {
    if (w.isPlayerAlive()) {
        PlayerObject& p = w.getPlayer();
        if (p.playerInControl() && p.pos.z >= pos.z) {
            doScript(w, false);
        }
        return p.pos.z < pos.z;
    }
    return true;
}

}  // namespace hiemalia
