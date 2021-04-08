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
SetSpeedScript::SetSpeedScript(const Point3D& pos, const std::string& prop)
    : ScriptObject(pos) {
    if (prop == "slow") {
        speed_ = 1;
        vel_ = 0.5;
    } else if (prop == "medium" || prop == "normal") {
        speed_ = 1.5;
        vel_ = 1;
    } else if (prop == "fast") {
        speed_ = 1.75;
        vel_ = 1;
    } else {
        never("invalid setspeed speed");
    }
}

void SetSpeedScript::doScript(GameWorld& w, bool instant) {
    w.setNewSpeed(speed_, vel_);
}

}  // namespace hiemalia
