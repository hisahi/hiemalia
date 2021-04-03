/****************************************************************************/
/*                                                                          */
/*   HIEMALIA SOURCE CODE (C) 2021      SAMPO HIPPELAINEN (HISAHI).         */
/*   SEE THE LICENSE FILE IN THE SOURCE ROOT DIRECTORY FOR LICENSE INFO.    */
/*                                                                          */
/****************************************************************************/
// game/sections.cc: implementation of game section resolver

#include "game/sections.hh"

#include "defs.hh"

namespace hiemalia {

const std::unordered_map<std::string, section_t> sectionMap = {
    {"plain", 0},
    {"plaintr", 1},
};

int getSectionCount() {
    int max = 0;
    for (auto& it : sectionMap) {
        int n = static_cast<int>(it.second);
        if (max < n) max = n;
    }
    return max;
}

section_t getSectionIdByName(const std::string& s) {
    auto it = sectionMap.find(s);
    if (it == sectionMap.end()) never("unrecognized section name");
    return it->second;
}

}  // namespace hiemalia
