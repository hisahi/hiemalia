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
void StageEndScript::doScript(GameWorld& w, bool instant) { w.endStage(); }

}  // namespace hiemalia
