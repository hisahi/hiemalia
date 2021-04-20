/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/gameend.cc: implementation of game end script object

#include "game/gameend.hh"

#include "game/world.hh"

namespace hiemalia {
void GameEndScript::doScript(GameWorld& w, bool instant) { w.endGame(); }

}  // namespace hiemalia
