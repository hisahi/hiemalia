/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/script.cc: implementation of script objects

#include "game/script.hh"

namespace hiemalia {
ScriptObject::ScriptObject() {}

bool ScriptObject::update(GameWorld& w, float delta) { return run(w); }

}  // namespace hiemalia
