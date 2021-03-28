/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/objects.hh: header file for game object spawns

#ifndef M_GAME_OBJECTS_HH
#define M_GAME_OBJECTS_HH

#include <memory>

#include "game/object.hh"

namespace hiemalia {
std::shared_ptr<GameObject> loadObjectSpawn(ModelPoint p,
                                            const std::string& name,
                                            const std::string& prop);
};  // namespace hiemalia

#endif  // M_GAME_OBJECTS_HH
