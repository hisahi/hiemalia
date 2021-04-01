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
    ScriptObject();
    bool update(GameWorld& w, float delta);
    virtual bool run(GameWorld& w) = 0;
    virtual ~ScriptObject() {}
};
};  // namespace hiemalia

#endif  // M_GAME_SCRIPT_HH
