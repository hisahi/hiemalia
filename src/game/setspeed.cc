/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/setspeed.cc: implementation of speed setting script object

#include "game/setspeed.hh"

#include "game/world.hh"

namespace hiemalia {
SetSpeedScript::SetSpeedScript(const std::string& prop) {
    if (prop == "slow") {
        speed_ = 0.875;
        vel_ = 0.5;
    } else if (prop == "medium" || prop == "normal") {
        speed_ = 1.25;
        vel_ = 1;
    } else if (prop == "fast") {
        speed_ = 1.75;
        vel_ = 1;
    } else {
        never("invalid setspeed speed");
    }
}

bool SetSpeedScript::run(GameWorld& w) {
    if (w.isPlayerAlive()) {
        PlayerObject& p = w.getPlayer();
        if (p.playerInControl() && p.pos.z >= pos.z) {
            w.setNewSpeed(speed_, vel_);
        }
        return p.pos.z < pos.z;
    }
    return true;
}

}  // namespace hiemalia
