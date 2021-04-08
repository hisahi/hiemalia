/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/script.hh: header file for script objects (game/script.cc)

#ifndef M_GAME_SCRIPT_HH
#define M_GAME_SCRIPT_HH

#include "game/object.hh"
#include "game/stage.hh"

namespace hiemalia {
class ScriptObject : public GameObject {
  public:
    ScriptObject(const Point3D& pos);
    bool update(GameWorld& w, float delta);
    virtual inline void doScript(GameWorld& w, bool instant) {}
    inline void instant(GameWorld& w) { doScript(w, true); }
    virtual ~ScriptObject() {}
};
};  // namespace hiemalia

#endif  // M_GAME_SCRIPT_HH
