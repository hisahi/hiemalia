/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/setspeed.hh: header file for speed setting script (game/setspeed.cc)

#ifndef M_GAME_SETSPEED_HH
#define M_GAME_SETSPEED_HH

#include "game/script.hh"

namespace hiemalia {
class SetSpeedScript : public ScriptObject {
  public:
    SetSpeedScript(const Point3D& pos, const std::string& prop);
    void doScript(GameWorld& w, bool instant);

  private:
    coord_t speed_;
    coord_t vel_;
};
};  // namespace hiemalia

#endif  // M_GAME_SETSPEED_HH
