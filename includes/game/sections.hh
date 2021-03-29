/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/sections.hh: header file for resolving game sections

#ifndef M_GAME_SECTIONS_HH
#define M_GAME_SECTIONS_HH

#include <unordered_map>

#include "game/world.hh"

namespace hiemalia {
extern const std::unordered_map<std::string, section_t> sectionMap;
int getSectionCount();
section_t getSectionIdByName(const std::string& s);
};  // namespace hiemalia

#endif  // M_GAME_SECTIONS_HH
